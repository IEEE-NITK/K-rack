#include <linux/fs.h>
#include <myfs.h>

int myfs_fill_sb(
    struct super_block *sb,
    void *data,
    int silent)
{
    struct inode *root = NULL;
    sb->s_magic = MYFS_MAGIC_NUMBER;
    /*
     *  MYFS_MAGIC_NUMBER must be defined in /include/api/linux/magic.h
     *  and it must be unique.
     */
    sb->s_op = &myfs_super_ops;

    root = new_inode(sb);
    if(!root)
    {
        pr_err("Inode allocation failed.\n");
        return -ENOMEM;
    }

    root->i_ino = 0;
    root->i_sb = sb;
    root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;

    inode_init_owner(root,NULL,S_IFDIR);

    sb->s_root = d_make_root(root);
    if (!sb->s_root)
    {
        pr_err("root creation failed\n");
        return -ENOMEM;
    }

    return 0;

}

void myfs_put_super(struct super_block *sb)
{
    pr_debug("aufs super block destroyed\n");
}

struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
};
