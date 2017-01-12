#include <linux/fs.h>
#include <myfs.h>
/*
 *  The prototype is:
    struct file_system_type
    {
 	    const char *name;
 	    int fs_flags;
 	    struct dentry *(*mount) (struct file_system_type *, int,
 		       const char *, void *);
 	    void (*kill_sb) (struct super_block *);
 	    struct module *owner;
 	    struct file_system_type * next;
 	    struct hlist_head fs_supers;

 	    struct lock_class_key s_lock_key;
 	    struct lock_class_key s_umount_key;
 	    struct lock_class_key s_vfs_rename_key;
 	    struct lock_class_key s_writers_key[SB_FREEZE_LEVELS];

 	    struct lock_class_key i_lock_key;
 	    struct lock_class_key i_mutex_key;
 	    struct lock_class_key i_mutex_dir_key;
    };

 *  An example usage is(ext2):
    static struct file_system_type ext2_fs_type = {
        .owner		= THIS_MODULE,
        .name		= "ext2",
        .mount		= ext2_mount,
        .kill_sb	= kill_block_super,
        .fs_flags	= FS_REQUIRES_DEV,
    };
    NOTE:
    1.  struct module *owner is commonly used at some structures and is not an
        operation at all; it is a pointer to the module that "owns"the
        structure. This field is used to prevent the module from being unloaded
        while its operations are in use. Almost all the time, it is simply
        initialized to THIS_MODULE, a macro defined in < linux/module.h> .
    2.  name refers to the name of the file system.
    3.  mount refers to a file system initialization function of the above given
        prototype.
    4.  The fs_flags field of a struct file_system_type is a bitmap, an OR of
        several possible flags with mostly obscure uses only. The flags are
        defined in fs.h.
        #define FS_REQUIRES_DEV		      1
        #define FS_BINARY_MOUNTDATA	      2
        #define FS_HAS_SUBTYPE		      4
        #define FS_USERNS_MOUNT		      8
        #define FS_USERNS_DEV_MOUNT       16
        #define FS_USERNS_VISIBLE	      32
        #define FS_RENAME_DOES_D_MOVE	  32768

        FS_REQUIRES_DEV
            The FS_REQUIRES_DEV flag says that this is not a virtual filesystem - an actual underlying block device is required. It is used in only two places: when /proc/filesystems is generated, its absence causes the filesystem type name to be prefixed by "nodev". And in fs/nfsd/export.c this flag is tested in the process of determining whether the filesystem can be exported via NFS. Earlier there were more uses.
        FS_BINARY_MOUNTDATA
            The FS_BINARY_MOUNTDATA flag (since 2.6.5) is set to tell the selinux code that the mount data is binary, and cannot be handled by the standard option parser. (This flag is set for afs, coda, nfs, smbfs.)
        FS_RENAME_DOES_D_MOVE
            The FS_RENAME_DOES_D_MOVE flag says that the low-level filesystem will handle d_move() during a rename(). FS will handle d_move() during rename() internally.
        FS_USERNS_MOUNT
            Can be mounted by userns root
        FS_USERNS_DEV_MOUNT
            A userns mount does not imply MNT_NODEV
        FS_USERNS_VISIBLE
            FS must already be visible
 */

struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name  = "mcryptfs",
    .mount = myfs_mount,
    .kill_sb = kill_block_super,
    .fs_flags = FS_REQUIRES_DEV,
};
