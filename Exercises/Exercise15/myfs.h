#include<linux/fs.h>
#define MYFS_MAGIC_NUMBER 0xAAFF2348

extern struct file_system_type myfs_type;
extern struct dentry* myfs_mount(
    struct file_system_type *fs_type,
    int flags,
    char const *dev,
    void *data );
extern int myfs_fill_sb(
    struct super_block *sb,
    void *data,
    int silent);
extern void myfs_put_super(struct super_block *sb);
extern struct super_operations const myfs_super_ops;
