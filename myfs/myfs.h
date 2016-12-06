#include<linux/fs.h>
#define MYFS_MAGIC_NUMBER 0xAAFF2348

extern struct file_system_type myfs_type;
/*
 *  myfs_type description type, needed to register a new filesystem.
 */

extern struct dentry* myfs_mount(
    struct file_system_type *fs_type,
    int flags,
    char const *dev,
    void *data );
/*
 *  The mounting function used to mount the filesystem.
 *  This function is used whenever the filesystem is mounted.
 */

extern int myfs_fill_sb(
    struct super_block *sb,
    void *data,
    int silent);
/*
 *  This function is used to fill a superblock with appropriate data.
 *  It fills the superblock structure with the necessary data and returns
 *  0 on success.
 */
extern void myfs_put_super(struct super_block *sb);
/*
 *  This function is called when a superblock needs to be destroyed.
 */
extern struct super_operations const myfs_super_ops;
/*
 *  This operations data structure handles all the operations that are
 *  performed on/by the superblock.
 */
