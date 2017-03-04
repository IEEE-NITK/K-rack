#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>

#define ROOT_FLAG 2

static struct kmem_cache * lfs_inode_cachep;

static struct super_operations kr_sops = {
        .alloc_inode = kr_alloc_inode,
}

static struct inode *kr_alloc_inode(struct super_block *sb)
{
        struct kr_inode_info *li;
        li = kmem_cache_alloc(lfs_inode_cachep, GFP_KERNEL);
        if(!li)
                return NULL;

}

static int kr_fill_super(struct super_block *sb, void *data, int silent)
{
        int err;
        struct inode *root;

        err = -ENOMEM;
        sb->s_flags = sb->s_flags & ~MS_POSIXACL;
        sb->s_op = &kr_sops;

        root = kr_iget(sb,ROOT_FLAG);
        if(IS_ERR(root)) {
                return err;
        }
        sb->s_root = d_make_root(root);
        return 0;
}

static struct dentry *kr_mount(struct file_system_type *kr_type,
        int flags, const char *dev_name, void *data)
{
        return mount_bdev(kr_type, flags, dev_name, data, kr_fill_super);
}

static struct file_system_type kr_fs_type = {
        .owner          = THIS_MODULE,
        .name           = "kr",
        .mount          = kr_mount,
        .kill_sb        = kill_block_super,
        .fs_flags       = FS_REQUIRES_DEV,
};

MODULE_ALIAS_FS("krack");
/*
 *      Things left:
 *      init_inodecache
 *      destroy_inodecache
 *      kr_fill_super
 *      kill_block_super
 *      ? kr_statfs
 *      kr_sops
 *      kr_inode_info
 */
static int __init init_kr_fs(void)
{
        int err;

        err = init_inodecache();
        if (err)
                return err;
        err = register_filesystem(&kr_fs_type);
        if (err)
                goto out;
        return 0;
out:
        destroy_inodecache();
        return err;
}

static void __exit exit_kr_fs(void)
{
        unregister_filesystem(&kr_fs_type);
        destroy_inodecache();
}

MODULE_AUTHOR("Adithya Bhat");
MODULE_DESCRIPTION("K-Rack");
MODULE_LICENSE("GPL");
module_init(init_kr_fs)
module_exit(exit_kr_fs)
