/*
 *  linux/fs/kr/file.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/file.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  kr fs regular file handling primitives
 *
 *  64-bit file support on 64-bit platforms by Jakub Jelinek
 * 	(jj@sunsite.ms.mff.cuni.cz)
 */

#include <linux/time.h>
#include <linux/pagemap.h>
#include <linux/dax.h>
#include <linux/quotaops.h>
#include <linux/iomap.h>
#include <linux/uio.h>
#include "kr.h"

#define kr_file_mmap	generic_file_mmap


/*
 * Called when filp is released. This happens when all file descriptors
 * for a single struct file are closed. Note that different open() calls
 * for the same file yield different struct file structures.
 */
static int kr_release_file (struct inode * inode, struct file * filp)
{
	if (filp->f_mode & FMODE_WRITE) {
		mutex_lock(&KR_I(inode)->truncate_mutex);
		kr_discard_reservation(inode);
		mutex_unlock(&KR_I(inode)->truncate_mutex);
	}
	return 0;
}

int kr_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	int ret;
	struct super_block *sb = file->f_mapping->host->i_sb;
	struct address_space *mapping = sb->s_bdev->bd_inode->i_mapping;

	ret = generic_file_fsync(file, start, end, datasync);
	if (ret == -EIO || test_and_clear_bit(AS_EIO, &mapping->flags)) {
		/* We don't really know where the IO error happened... */
		kr_error(sb, __func__,
			   "detected IO error when writing metadata buffers");
		ret = -EIO;
	}
	return ret;
}

static ssize_t kr_file_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	return generic_file_read_iter(iocb, to);
}

static ssize_t kr_file_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	return generic_file_write_iter(iocb, from);
}

const struct file_operations kr_file_operations = {
	.llseek		= generic_file_llseek,
	.read_iter	= kr_file_read_iter,
	.write_iter	= kr_file_write_iter,
	.unlocked_ioctl = kr_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= kr_compat_ioctl,
#endif
	.mmap		= kr_file_mmap,
	.open		= dquot_file_open,
	.release	= kr_release_file,
	.fsync		= kr_fsync,
	.get_unmapped_area = thp_get_unmapped_area,
	.splice_read	= generic_file_splice_read,
	.splice_write	= iter_file_splice_write,
};

const struct inode_operations kr_file_inode_operations = {
	.setattr	= kr_setattr,
	.fiemap		= kr_fiemap,
};
