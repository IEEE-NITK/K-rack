/*
 * Demonstrate a trivial filesystem using libfs.
 *
 * Copyright 2002, 2003 Jonathan Corbet <corbet@lwn.net>
 * This file may be redistributed under the terms of the GNU GPL.
 *
 * Chances are that this code will crash your system, delete your
 * nethack high scores, and set your disk drives on fire.  You have
 * been warned.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <asm/page.h> 	/* PAGE_SIZE */
#include <linux/fs.h>     	/* This is where libfs stuff is declared */
#include <asm/atomic.h>
#include <asm/uaccess.h>	/* copy_to_user */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jonathan Corbet");

#define LFS_MAGIC 0x19980122

static struct inode *lfs_make_inode(struct super_block *sb, int mode)
{
	struct inode *ret = new_inode(sb);

	if (ret) {
		ret->i_mode = mode;
		ret->i_uid.val = ret->i_gid.val = 0;

		ret->i_blocks = 0;
		ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
	}
	return ret;
}

static int lfs_open(struct inode *inode, struct file *filp)
{
	filp->private_data = inode->i_private;
	return 0;
}

#define TMPSIZE 20
static ssize_t lfs_read_file(struct file *filp, char *buf,
		size_t count, loff_t *offset)
{
	atomic_t *counter = (atomic_t *) filp->private_data;
	int v, len;
	char tmp[TMPSIZE];
	v = atomic_read(counter);
	if (*offset > 0)
		v -= 1;  /* the value returned when offset was zero */
	else
		atomic_inc(counter);
	len = sprintf(tmp, TMPSIZE, "%d\n", v);
	if (*offset > len)
		return 0;
	if (count > len - *offset)
		count = len - *offset;

	if (copy_to_user(buf, tmp + *offset, count))
		return -EFAULT;
	*offset += count;
	return count;
}

static ssize_t lfs_write_file(struct file *filp, const char *buf,
		size_t count, loff_t *offset)
{
	atomic_t *counter = (atomic_t *) filp->private_data;
	char tmp[TMPSIZE];

	if (*offset != 0)
		return -EINVAL;

	if (count >= TMPSIZE)
		return -EINVAL;
	memset(tmp, 0, TMPSIZE);
	if (copy_from_user(tmp, buf, count))
		return -EFAULT;

	atomic_set(counter, simple_strtol(tmp, NULL, 10));
	return count;
}

static struct file_operations lfs_file_ops = {
	.open	= lfs_open,
	.read 	= lfs_read_file,
	.write  = lfs_write_file,
};

static struct dentry *lfs_create_file (struct super_block *sb,
		struct dentry *dir, const char *name,
		atomic_t *counter)
{
	struct dentry *dentry;
	struct inode *inode;
	struct qstr qname;

	qname.name = name;
	qname.len = strlen (name);
	qname.hash = full_name_hash(name, qname.len);

	dentry = d_alloc(dir, &qname);
	if (! dentry)
		goto out;
	inode = lfs_make_inode(sb, S_IFREG | 00777);
	if (! inode)
		goto out_dput;
	inode->i_fop = &simple_dir_operations;
	inode->i_private = counter;

	d_add(dentry, inode);
	return dentry;

  out_dput:
	dput(dentry);
  out:
	return 0;
}

static atomic_t counter, subcounter;

static void lfs_create_files (struct super_block *sb, struct dentry *root)
{
	atomic_set(&counter, 0);
	lfs_create_file(sb, root, "counter", &counter);
}

static struct super_operations lfs_s_ops = {
	.statfs		= simple_statfs,
	.drop_inode	= generic_delete_inode,
};

static int lfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	struct dentry *root_dentry;

	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic = LFS_MAGIC;
	sb->s_op = &lfs_s_ops;

	root = lfs_make_inode (sb, S_IFDIR | 0777);
	if (! root)
		goto out;
	root->i_op = &simple_dir_inode_operations;
	root->i_fop = &simple_dir_operations;

	root_dentry = d_make_root(root);

	if (! root_dentry)
		goto out_iput;
	sb->s_root = root_dentry;
	lfs_create_files (sb, root_dentry);
	return 0;

  out_iput:
	iput(root);
  out:
	return -ENOMEM;
}

static struct dentry *lfs_get_super(struct file_system_type *fst,
		int flags, const char *devname, void *data)
{
	return mount_bdev(fst, flags, devname, data, lfs_fill_super);
}

static struct file_system_type lfs_type = {
	.owner 		= THIS_MODULE,
	.name		= "lfs",
	.mount		= lfs_get_super,
	.kill_sb	= kill_litter_super,
};

static int __init lfs_init(void)
{
	return register_filesystem(&lfs_type);
}

static void __exit lfs_exit(void)
{
	unregister_filesystem(&lfs_type);
}

module_init(lfs_init);
module_exit(lfs_exit);
