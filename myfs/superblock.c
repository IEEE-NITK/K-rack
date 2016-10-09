#include <linux/fs.h>

int myfs_fill_sb(struct super_block *sb, void *data, int silent)
{
    struct inode *root = NULL;
    sb->s_magic = MYFS_MAGIC_NUMBER;
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

    sb
}
