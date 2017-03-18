/*
 *  linux/fs/kr/dir.c
 *
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/fs/minix/dir.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  kr directory handling functions
 *
 *  Big-endian to little-endian byte-swapping/bitmaps by
 *        David S. Miller (davem@caip.rutgers.edu), 1995
 *
 * All code that works with directory layout had been switched to pagecache
 * and moved here. AV
 */

#include "kr.h"
#include <linux/buffer_head.h>
#include <linux/pagemap.h>
#include <linux/swap.h>

typedef struct kr_dir_entry_2 kr_dirent;

/*
 * Tests against MAX_REC_LEN etc were put in place for 64k block
 * sizes; if that is not possible on this arch, we can skip
 * those tests and speed things up.
 */
static inline unsigned kr_rec_len_from_disk(__le16 dlen)
{
	unsigned len = le16_to_cpu(dlen);

#if (PAGE_SIZE >= 65536)
	if (len == KR_MAX_REC_LEN)
		return 1 << 16;
#endif
	return len;
}

static inline __le16 kr_rec_len_to_disk(unsigned len)
{
#if (PAGE_SIZE >= 65536)
	if (len == (1 << 16))
		return cpu_to_le16(KR_MAX_REC_LEN);
	else
		BUG_ON(len > (1 << 16));
#endif
	return cpu_to_le16(len);
}

/*
 * kr uses block-sized chunks. Arguably, sector-sized ones would be
 * more robust, but we have what we have
 */
static inline unsigned kr_chunk_size(struct inode *inode)
{
	return inode->i_sb->s_blocksize;
}

static inline void kr_put_page(struct page *page)
{
	kunmap(page);
	put_page(page);
}

/*
 * Return the offset into page `page_nr' of the last valid
 * byte in that page, plus one.
 */
static unsigned
kr_last_byte(struct inode *inode, unsigned long page_nr)
{
	unsigned last_byte = inode->i_size;

	last_byte -= page_nr << PAGE_SHIFT;
	if (last_byte > PAGE_SIZE)
		last_byte = PAGE_SIZE;
	return last_byte;
}

static int kr_commit_chunk(struct page *page, loff_t pos, unsigned len)
{
	struct address_space *mapping = page->mapping;
	struct inode *dir = mapping->host;
	int err = 0;

	dir->i_version++;
	block_write_end(NULL, mapping, pos, len, len, page, NULL);

	if (pos+len > dir->i_size) {
		i_size_write(dir, pos+len);
		mark_inode_dirty(dir);
	}

	if (IS_DIRSYNC(dir)) {
		err = write_one_page(page, 1);
		if (!err)
			err = sync_inode_metadata(dir, 1);
	} else {
		unlock_page(page);
	}

	return err;
}

static bool kr_check_page(struct page *page, int quiet)
{
	struct inode *dir = page->mapping->host;
	struct super_block *sb = dir->i_sb;
	unsigned chunk_size = kr_chunk_size(dir);
	char *kaddr = page_address(page);
	u32 max_inumber = le32_to_cpu(KR_SB(sb)->s_es->s_inodes_count);
	unsigned offs, rec_len;
	unsigned limit = PAGE_SIZE;
	kr_dirent *p;
	char *error;

	if ((dir->i_size >> PAGE_SHIFT) == page->index) {
		limit = dir->i_size & ~PAGE_MASK;
		if (limit & (chunk_size - 1))
			goto Ebadsize;
		if (!limit)
			goto out;
	}
	for (offs = 0; offs <= limit - KR_DIR_REC_LEN(1); offs += rec_len) {
		p = (kr_dirent *)(kaddr + offs);
		rec_len = kr_rec_len_from_disk(p->rec_len);

		if (unlikely(rec_len < KR_DIR_REC_LEN(1)))
			goto Eshort;
		if (unlikely(rec_len & 3))
			goto Ealign;
		if (unlikely(rec_len < KR_DIR_REC_LEN(p->name_len)))
			goto Enamelen;
		if (unlikely(((offs + rec_len - 1) ^ offs) & ~(chunk_size-1)))
			goto Espan;
		if (unlikely(le32_to_cpu(p->inode) > max_inumber))
			goto Einumber;
	}
	if (offs != limit)
		goto Eend;
out:
	SetPageChecked(page);
	return true;

	/* Too bad, we had an error */

Ebadsize:
	if (!quiet)
		kr_error(sb, __func__,
			"size of directory #%lu is not a multiple "
			"of chunk size", dir->i_ino);
	goto fail;
Eshort:
	error = "rec_len is smaller than minimal";
	goto bad_entry;
Ealign:
	error = "unaligned directory entry";
	goto bad_entry;
Enamelen:
	error = "rec_len is too small for name_len";
	goto bad_entry;
Espan:
	error = "directory entry across blocks";
	goto bad_entry;
Einumber:
	error = "inode out of bounds";
bad_entry:
	if (!quiet)
		kr_error(sb, __func__, "bad entry in directory #%lu: : %s - "
			"offset=%lu, inode=%lu, rec_len=%d, name_len=%d",
			dir->i_ino, error, (page->index<<PAGE_SHIFT)+offs,
			(unsigned long) le32_to_cpu(p->inode),
			rec_len, p->name_len);
	goto fail;
Eend:
	if (!quiet) {
		p = (kr_dirent *)(kaddr + offs);
		kr_error(sb, "kr_check_page",
			"entry in directory #%lu spans the page boundary"
			"offset=%lu, inode=%lu",
			dir->i_ino, (page->index<<PAGE_SHIFT)+offs,
			(unsigned long) le32_to_cpu(p->inode));
	}
fail:
	SetPageError(page);
	return false;
}

static struct page * kr_get_page(struct inode *dir, unsigned long n,
				   int quiet)
{
	struct address_space *mapping = dir->i_mapping;
	struct page *page = read_mapping_page(mapping, n, NULL);
	if (!IS_ERR(page)) {
		kmap(page);
		if (unlikely(!PageChecked(page))) {
			if (PageError(page) || !kr_check_page(page, quiet))
				goto fail;
		}
	}
	return page;

fail:
	kr_put_page(page);
	return ERR_PTR(-EIO);
}

/*
 * NOTE! unlike strncmp, kr_match returns 1 for success, 0 for failure.
 *
 * len <= KR_NAME_LEN and de != NULL are guaranteed by caller.
 */
static inline int kr_match (int len, const char * const name,
					struct kr_dir_entry_2 * de)
{
	if (len != de->name_len)
		return 0;
	if (!de->inode)
		return 0;
	return !memcmp(name, de->name, len);
}

/*
 * p is at least 6 bytes before the end of page
 */
static inline kr_dirent *kr_next_entry(kr_dirent *p)
{
	return (kr_dirent *)((char *)p +
			kr_rec_len_from_disk(p->rec_len));
}

static inline unsigned
kr_validate_entry(char *base, unsigned offset, unsigned mask)
{
	kr_dirent *de = (kr_dirent*)(base + offset);
	kr_dirent *p = (kr_dirent*)(base + (offset&mask));
	while ((char*)p < (char*)de) {
		if (p->rec_len == 0)
			break;
		p = kr_next_entry(p);
	}
	return (char *)p - base;
}

static unsigned char kr_filetype_table[KR_FT_MAX] = {
	[KR_FT_UNKNOWN]	= DT_UNKNOWN,
	[KR_FT_REG_FILE]	= DT_REG,
	[KR_FT_DIR]		= DT_DIR,
	[KR_FT_CHRDEV]	= DT_CHR,
	[KR_FT_BLKDEV]	= DT_BLK,
	[KR_FT_FIFO]		= DT_FIFO,
	[KR_FT_SOCK]		= DT_SOCK,
	[KR_FT_SYMLINK]	= DT_LNK,
};

#define S_SHIFT 12
static unsigned char kr_type_by_mode[S_IFMT >> S_SHIFT] = {
	[S_IFREG >> S_SHIFT]	= KR_FT_REG_FILE,
	[S_IFDIR >> S_SHIFT]	= KR_FT_DIR,
	[S_IFCHR >> S_SHIFT]	= KR_FT_CHRDEV,
	[S_IFBLK >> S_SHIFT]	= KR_FT_BLKDEV,
	[S_IFIFO >> S_SHIFT]	= KR_FT_FIFO,
	[S_IFSOCK >> S_SHIFT]	= KR_FT_SOCK,
	[S_IFLNK >> S_SHIFT]	= KR_FT_SYMLINK,
};

static inline void kr_set_de_type(kr_dirent *de, struct inode *inode)
{
	umode_t mode = inode->i_mode;
	if (KR_HAS_INCOMPAT_FEATURE(inode->i_sb, KR_FEATURE_INCOMPAT_FILETYPE))
		de->file_type = kr_type_by_mode[(mode & S_IFMT)>>S_SHIFT];
	else
		de->file_type = 0;
}

static int
kr_readdir(struct file *file, struct dir_context *ctx)
{
	loff_t pos = ctx->pos;
	struct inode *inode = file_inode(file);
	struct super_block *sb = inode->i_sb;
	unsigned int offset = pos & ~PAGE_MASK;
	unsigned long n = pos >> PAGE_SHIFT;
	unsigned long npages = dir_pages(inode);
	unsigned chunk_mask = ~(kr_chunk_size(inode)-1);
	unsigned char *types = NULL;
	int need_revalidate = file->f_version != inode->i_version;

	if (pos > inode->i_size - KR_DIR_REC_LEN(1))
		return 0;

	if (KR_HAS_INCOMPAT_FEATURE(sb, KR_FEATURE_INCOMPAT_FILETYPE))
		types = kr_filetype_table;

	for ( ; n < npages; n++, offset = 0) {
		char *kaddr, *limit;
		kr_dirent *de;
		struct page *page = kr_get_page(inode, n, 0);

		if (IS_ERR(page)) {
			kr_error(sb, __func__,
				   "bad page in #%lu",
				   inode->i_ino);
			ctx->pos += PAGE_SIZE - offset;
			return PTR_ERR(page);
		}
		kaddr = page_address(page);
		if (unlikely(need_revalidate)) {
			if (offset) {
				offset = kr_validate_entry(kaddr, offset, chunk_mask);
				ctx->pos = (n<<PAGE_SHIFT) + offset;
			}
			file->f_version = inode->i_version;
			need_revalidate = 0;
		}
		de = (kr_dirent *)(kaddr+offset);
		limit = kaddr + kr_last_byte(inode, n) - KR_DIR_REC_LEN(1);
		for ( ;(char*)de <= limit; de = kr_next_entry(de)) {
			if (de->rec_len == 0) {
				kr_error(sb, __func__,
					"zero-length directory entry");
				kr_put_page(page);
				return -EIO;
			}
			if (de->inode) {
				unsigned char d_type = DT_UNKNOWN;

				if (types && de->file_type < KR_FT_MAX)
					d_type = types[de->file_type];

				if (!dir_emit(ctx, de->name, de->name_len,
						le32_to_cpu(de->inode),
						d_type)) {
					kr_put_page(page);
					return 0;
				}
			}
			ctx->pos += kr_rec_len_from_disk(de->rec_len);
		}
		kr_put_page(page);
	}
	return 0;
}

/*
 *	kr_find_entry()
 *
 * finds an entry in the specified directory with the wanted name. It
 * returns the page in which the entry was found (as a parameter - res_page),
 * and the entry itself. Page is returned mapped and unlocked.
 * Entry is guaranteed to be valid.
 */
struct kr_dir_entry_2 *kr_find_entry (struct inode *dir,
			const struct qstr *child, struct page **res_page)
{
	const char *name = child->name;
	int namelen = child->len;
	unsigned reclen = KR_DIR_REC_LEN(namelen);
	unsigned long start, n;
	unsigned long npages = dir_pages(dir);
	struct page *page = NULL;
	struct kr_inode_info *ei = KR_I(dir);
	kr_dirent * de;
	int dir_has_error = 0;

	if (npages == 0)
		goto out;

	/* OFFSET_CACHE */
	*res_page = NULL;

	start = ei->i_dir_start_lookup;
	if (start >= npages)
		start = 0;
	n = start;
	do {
		char *kaddr;
		page = kr_get_page(dir, n, dir_has_error);
		if (!IS_ERR(page)) {
			kaddr = page_address(page);
			de = (kr_dirent *) kaddr;
			kaddr += kr_last_byte(dir, n) - reclen;
			while ((char *) de <= kaddr) {
				if (de->rec_len == 0) {
					kr_error(dir->i_sb, __func__,
						"zero-length directory entry");
					kr_put_page(page);
					goto out;
				}
				if (kr_match (namelen, name, de))
					goto found;
				de = kr_next_entry(de);
			}
			kr_put_page(page);
		} else
			dir_has_error = 1;

		if (++n >= npages)
			n = 0;
		/* next page is past the blocks we've got */
		if (unlikely(n > (dir->i_blocks >> (PAGE_SHIFT - 9)))) {
			kr_error(dir->i_sb, __func__,
				"dir %lu size %lld exceeds block count %llu",
				dir->i_ino, dir->i_size,
				(unsigned long long)dir->i_blocks);
			goto out;
		}
	} while (n != start);
out:
	return NULL;

found:
	*res_page = page;
	ei->i_dir_start_lookup = n;
	return de;
}

struct kr_dir_entry_2 * kr_dotdot (struct inode *dir, struct page **p)
{
	struct page *page = kr_get_page(dir, 0, 0);
	kr_dirent *de = NULL;

	if (!IS_ERR(page)) {
		de = kr_next_entry((kr_dirent *) page_address(page));
		*p = page;
	}
	return de;
}

ino_t kr_inode_by_name(struct inode *dir, const struct qstr *child)
{
	ino_t res = 0;
	struct kr_dir_entry_2 *de;
	struct page *page;

	de = kr_find_entry (dir, child, &page);
	if (de) {
		res = le32_to_cpu(de->inode);
		kr_put_page(page);
	}
	return res;
}

static int kr_prepare_chunk(struct page *page, loff_t pos, unsigned len)
{
	return __block_write_begin(page, pos, len, kr_get_block);
}

/* Releases the page */
void kr_set_link(struct inode *dir, struct kr_dir_entry_2 *de,
		   struct page *page, struct inode *inode, int update_times)
{
	loff_t pos = page_offset(page) +
			(char *) de - (char *) page_address(page);
	unsigned len = kr_rec_len_from_disk(de->rec_len);
	int err;

	lock_page(page);
	err = kr_prepare_chunk(page, pos, len);
	BUG_ON(err);
	de->inode = cpu_to_le32(inode->i_ino);
	kr_set_de_type(de, inode);
	err = kr_commit_chunk(page, pos, len);
	kr_put_page(page);
	if (update_times)
		dir->i_mtime = dir->i_ctime = current_time(dir);
	KR_I(dir)->i_flags &= ~KR_BTREE_FL;
	mark_inode_dirty(dir);
}

/*
 *	Parent is locked.
 */
int kr_add_link (struct dentry *dentry, struct inode *inode)
{
	struct inode *dir = d_inode(dentry->d_parent);
	const char *name = dentry->d_name.name;
	int namelen = dentry->d_name.len;
	unsigned chunk_size = kr_chunk_size(dir);
	unsigned reclen = KR_DIR_REC_LEN(namelen);
	unsigned short rec_len, name_len;
	struct page *page = NULL;
	kr_dirent * de;
	unsigned long npages = dir_pages(dir);
	unsigned long n;
	char *kaddr;
	loff_t pos;
	int err;

	/*
	 * We take care of directory expansion in the same loop.
	 * This code plays outside i_size, so it locks the page
	 * to protect that region.
	 */
	for (n = 0; n <= npages; n++) {
		char *dir_end;

		page = kr_get_page(dir, n, 0);
		err = PTR_ERR(page);
		if (IS_ERR(page))
			goto out;
		lock_page(page);
		kaddr = page_address(page);
		dir_end = kaddr + kr_last_byte(dir, n);
		de = (kr_dirent *)kaddr;
		kaddr += PAGE_SIZE - reclen;
		while ((char *)de <= kaddr) {
			if ((char *)de == dir_end) {
				/* We hit i_size */
				name_len = 0;
				rec_len = chunk_size;
				de->rec_len = kr_rec_len_to_disk(chunk_size);
				de->inode = 0;
				goto got_it;
			}
			if (de->rec_len == 0) {
				kr_error(dir->i_sb, __func__,
					"zero-length directory entry");
				err = -EIO;
				goto out_unlock;
			}
			err = -EEXIST;
			if (kr_match (namelen, name, de))
				goto out_unlock;
			name_len = KR_DIR_REC_LEN(de->name_len);
			rec_len = kr_rec_len_from_disk(de->rec_len);
			if (!de->inode && rec_len >= reclen)
				goto got_it;
			if (rec_len >= name_len + reclen)
				goto got_it;
			de = (kr_dirent *) ((char *) de + rec_len);
		}
		unlock_page(page);
		kr_put_page(page);
	}
	BUG();
	return -EINVAL;

got_it:
	pos = page_offset(page) +
		(char*)de - (char*)page_address(page);
	err = kr_prepare_chunk(page, pos, rec_len);
	if (err)
		goto out_unlock;
	if (de->inode) {
		kr_dirent *de1 = (kr_dirent *) ((char *) de + name_len);
		de1->rec_len = kr_rec_len_to_disk(rec_len - name_len);
		de->rec_len = kr_rec_len_to_disk(name_len);
		de = de1;
	}
	de->name_len = namelen;
	memcpy(de->name, name, namelen);
	de->inode = cpu_to_le32(inode->i_ino);
	kr_set_de_type (de, inode);
	err = kr_commit_chunk(page, pos, rec_len);
	dir->i_mtime = dir->i_ctime = current_time(dir);
	KR_I(dir)->i_flags &= ~KR_BTREE_FL;
	mark_inode_dirty(dir);
	/* OFFSET_CACHE */
out_put:
	kr_put_page(page);
out:
	return err;
out_unlock:
	unlock_page(page);
	goto out_put;
}

/*
 * kr_delete_entry deletes a directory entry by merging it with the
 * previous entry. Page is up-to-date. Releases the page.
 */
int kr_delete_entry (struct kr_dir_entry_2 * dir, struct page * page )
{
	struct inode *inode = page->mapping->host;
	char *kaddr = page_address(page);
	unsigned from = ((char*)dir - kaddr) & ~(kr_chunk_size(inode)-1);
	unsigned to = ((char *)dir - kaddr) +
				kr_rec_len_from_disk(dir->rec_len);
	loff_t pos;
	kr_dirent * pde = NULL;
	kr_dirent * de = (kr_dirent *) (kaddr + from);
	int err;

	while ((char*)de < (char*)dir) {
		if (de->rec_len == 0) {
			kr_error(inode->i_sb, __func__,
				"zero-length directory entry");
			err = -EIO;
			goto out;
		}
		pde = de;
		de = kr_next_entry(de);
	}
	if (pde)
		from = (char*)pde - (char*)page_address(page);
	pos = page_offset(page) + from;
	lock_page(page);
	err = kr_prepare_chunk(page, pos, to - from);
	BUG_ON(err);
	if (pde)
		pde->rec_len = kr_rec_len_to_disk(to - from);
	dir->inode = 0;
	err = kr_commit_chunk(page, pos, to - from);
	inode->i_ctime = inode->i_mtime = current_time(inode);
	KR_I(inode)->i_flags &= ~KR_BTREE_FL;
	mark_inode_dirty(inode);
out:
	kr_put_page(page);
	return err;
}

/*
 * Set the first fragment of directory.
 */
int kr_make_empty(struct inode *inode, struct inode *parent)
{
	struct page *page = grab_cache_page(inode->i_mapping, 0);
	unsigned chunk_size = kr_chunk_size(inode);
	struct kr_dir_entry_2 * de;
	int err;
	void *kaddr;

	if (!page)
		return -ENOMEM;

	err = kr_prepare_chunk(page, 0, chunk_size);
	if (err) {
		unlock_page(page);
		goto fail;
	}
	kaddr = kmap_atomic(page);
	memset(kaddr, 0, chunk_size);
	de = (struct kr_dir_entry_2 *)kaddr;
	de->name_len = 1;
	de->rec_len = kr_rec_len_to_disk(KR_DIR_REC_LEN(1));
	memcpy (de->name, ".\0\0", 4);
	de->inode = cpu_to_le32(inode->i_ino);
	kr_set_de_type (de, inode);

	de = (struct kr_dir_entry_2 *)(kaddr + KR_DIR_REC_LEN(1));
	de->name_len = 2;
	de->rec_len = kr_rec_len_to_disk(chunk_size - KR_DIR_REC_LEN(1));
	de->inode = cpu_to_le32(parent->i_ino);
	memcpy (de->name, "..\0", 4);
	kr_set_de_type (de, inode);
	kunmap_atomic(kaddr);
	err = kr_commit_chunk(page, 0, chunk_size);
fail:
	put_page(page);
	return err;
}

/*
 * routine to check that the specified directory is empty (for rmdir)
 */
int kr_empty_dir (struct inode * inode)
{
	struct page *page = NULL;
	unsigned long i, npages = dir_pages(inode);
	int dir_has_error = 0;

	for (i = 0; i < npages; i++) {
		char *kaddr;
		kr_dirent * de;
		page = kr_get_page(inode, i, dir_has_error);

		if (IS_ERR(page)) {
			dir_has_error = 1;
			continue;
		}

		kaddr = page_address(page);
		de = (kr_dirent *)kaddr;
		kaddr += kr_last_byte(inode, i) - KR_DIR_REC_LEN(1);

		while ((char *)de <= kaddr) {
			if (de->rec_len == 0) {
				kr_error(inode->i_sb, __func__,
					"zero-length directory entry");
				printk("kaddr=%p, de=%p\n", kaddr, de);
				goto not_empty;
			}
			if (de->inode != 0) {
				/* check for . and .. */
				if (de->name[0] != '.')
					goto not_empty;
				if (de->name_len > 2)
					goto not_empty;
				if (de->name_len < 2) {
					if (de->inode !=
					    cpu_to_le32(inode->i_ino))
						goto not_empty;
				} else if (de->name[1] != '.')
					goto not_empty;
			}
			de = kr_next_entry(de);
		}
		kr_put_page(page);
	}
	return 1;

not_empty:
	kr_put_page(page);
	return 0;
}

const struct file_operations kr_dir_operations = {
	.llseek		= generic_file_llseek,
	.read		= generic_read_dir,
	.iterate_shared	= kr_readdir,
	.unlocked_ioctl = kr_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= kr_compat_ioctl,
#endif
	.fsync		= kr_fsync,
};
