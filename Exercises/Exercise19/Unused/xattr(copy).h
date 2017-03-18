/*
  File: linux/ext2_xattr.h

  On-disk format of extended attributes for the ext2 filesystem.

  (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
  Edit: For K-rack
  	by, Adithya Bhat, <dth.bht@gmail.com>
*/

#include <linux/init.h>
#include <linux/xattr.h>

static inline int
ext2_xattr_get(struct inode *inode, int name_index,
	       const char *name, void *buffer, size_t size)
{
	return -EOPNOTSUPP;
}

static inline int
ext2_xattr_set(struct inode *inode, int name_index, const char *name,
	       const void *value, size_t size, int flags)
{
	return -EOPNOTSUPP;
}

static inline void
ext2_xattr_delete_inode(struct inode *inode)
{
}

static inline void ext2_xattr_destroy_cache(struct mb_cache *cache)
{
}

#define ext2_xattr_handlers NULL

static inline int ext2_init_security(struct inode *inode, struct inode *dir,
				     const struct qstr *qstr)
{
	return 0;
}
