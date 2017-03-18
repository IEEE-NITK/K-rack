/*
  File: fs/ext2/acl.h

  (C) 2001 Andreas Gruenbacher, <a.gruenbacher@computer.org>
  Edit: For K-rack,
  	by, Adithya Bhat, <dth.bht@gmail.com>
*/

#include <linux/posix_acl_xattr.h>

#define ext2_get_acl	NULL
#define ext2_set_acl	NULL

static inline int ext2_init_acl (struct inode *inode, struct inode *dir)
{
	return 0;
}
