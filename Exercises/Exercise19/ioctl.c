/*
 * linux/fs/kr/ioctl.c
 *
 * Copyright (C) 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 */

#include "kr.h"
#include <linux/capability.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/compat.h>
#include <linux/mount.h>
#include <asm/current.h>
#include <asm/uaccess.h>


long kr_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	struct kr_inode_info *ei = KR_I(inode);
	unsigned int flags;
	unsigned short rsv_window_size;
	int ret;

	kr_debug ("cmd = %u, arg = %lu\n", cmd, arg);

	switch (cmd) {
	case KR_IOC_GETFLAGS:
		kr_get_inode_flags(ei);
		flags = ei->i_flags & KR_FL_USER_VISIBLE;
		return put_user(flags, (int __user *) arg);
	case KR_IOC_SETFLAGS: {
		unsigned int oldflags;

		ret = mnt_want_write_file(filp);
		if (ret)
			return ret;

		if (!inode_owner_or_capable(inode)) {
			ret = -EACCES;
			goto setflags_out;
		}

		if (get_user(flags, (int __user *) arg)) {
			ret = -EFAULT;
			goto setflags_out;
		}

		flags = kr_mask_flags(inode->i_mode, flags);

		inode_lock(inode);
		/* Is it quota file? Do not allow user to mess with it */
		if (IS_NOQUOTA(inode)) {
			inode_unlock(inode);
			ret = -EPERM;
			goto setflags_out;
		}
		oldflags = ei->i_flags;

		/*
		 * The IMMUTABLE and APPEND_ONLY flags can only be changed by
		 * the relevant capability.
		 *
		 * This test looks nicer. Thanks to Pauline Middelink
		 */
		if ((flags ^ oldflags) & (KR_APPEND_FL | KR_IMMUTABLE_FL)) {
			if (!capable(CAP_LINUX_IMMUTABLE)) {
				inode_unlock(inode);
				ret = -EPERM;
				goto setflags_out;
			}
		}

		flags = flags & KR_FL_USER_MODIFIABLE;
		flags |= oldflags & ~KR_FL_USER_MODIFIABLE;
		ei->i_flags = flags;

		kr_set_inode_flags(inode);
		inode->i_ctime = current_time(inode);
		inode_unlock(inode);

		mark_inode_dirty(inode);
setflags_out:
		mnt_drop_write_file(filp);
		return ret;
	}
	case KR_IOC_GETVERSION:
		return put_user(inode->i_generation, (int __user *) arg);
	case KR_IOC_SETVERSION: {
		__u32 generation;

		if (!inode_owner_or_capable(inode))
			return -EPERM;
		ret = mnt_want_write_file(filp);
		if (ret)
			return ret;
		if (get_user(generation, (int __user *) arg)) {
			ret = -EFAULT;
			goto setversion_out;
		}

		inode_lock(inode);
		inode->i_ctime = current_time(inode);
		inode->i_generation = generation;
		inode_unlock(inode);

		mark_inode_dirty(inode);
setversion_out:
		mnt_drop_write_file(filp);
		return ret;
	}
	case KR_IOC_GETRSVSZ:
		if (test_opt(inode->i_sb, RESERVATION)
			&& S_ISREG(inode->i_mode)
			&& ei->i_block_alloc_info) {
			rsv_window_size = ei->i_block_alloc_info->rsv_window_node.rsv_goal_size;
			return put_user(rsv_window_size, (int __user *)arg);
		}
		return -ENOTTY;
	case KR_IOC_SETRSVSZ: {

		if (!test_opt(inode->i_sb, RESERVATION) ||!S_ISREG(inode->i_mode))
			return -ENOTTY;

		if (!inode_owner_or_capable(inode))
			return -EACCES;

		if (get_user(rsv_window_size, (int __user *)arg))
			return -EFAULT;

		ret = mnt_want_write_file(filp);
		if (ret)
			return ret;

		if (rsv_window_size > KR_MAX_RESERVE_BLOCKS)
			rsv_window_size = KR_MAX_RESERVE_BLOCKS;

		/*
		 * need to allocate reservation structure for this inode
		 * before set the window size
		 */
		/*
		 * XXX What lock should protect the rsv_goal_size?
		 * Accessed in kr_get_block only.  ext3 uses i_truncate.
		 */
		mutex_lock(&ei->truncate_mutex);
		if (!ei->i_block_alloc_info)
			kr_init_block_alloc_info(inode);

		if (ei->i_block_alloc_info){
			struct kr_reserve_window_node *rsv = &ei->i_block_alloc_info->rsv_window_node;
			rsv->rsv_goal_size = rsv_window_size;
		}
		mutex_unlock(&ei->truncate_mutex);
		mnt_drop_write_file(filp);
		return 0;
	}
	default:
		return -ENOTTY;
	}
}

#ifdef CONFIG_COMPAT
long kr_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	/* These are just misnamed, they actually get/put from/to user an int */
	switch (cmd) {
	case KR_IOC32_GETFLAGS:
		cmd = KR_IOC_GETFLAGS;
		break;
	case KR_IOC32_SETFLAGS:
		cmd = KR_IOC_SETFLAGS;
		break;
	case KR_IOC32_GETVERSION:
		cmd = KR_IOC_GETVERSION;
		break;
	case KR_IOC32_SETVERSION:
		cmd = KR_IOC_SETVERSION;
		break;
	default:
		return -ENOIOCTLCMD;
	}
	return kr_ioctl(file, cmd, (unsigned long) compat_ptr(arg));
}
#endif
