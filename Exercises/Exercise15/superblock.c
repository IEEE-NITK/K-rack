#include <linux/fs.h>
#include <myfs.h>

int myfs_fill_sb(
    struct super_block *sb,
    void *data,
    int silent)
{
    struct inode *root = NULL;
    /*
     *  Creating an inode for the root directory.
        struct inode {
            umode_t                 i_mode;
            unsigned short          i_opflags;
            kuid_t                  i_uid;
            kgid_t                  i_gid;
            unsigned int            i_flags;

        #ifdef CONFIG_FS_POSIX_ACL
            struct posix_acl        *i_acl;
            struct posix_acl        *i_default_acl;
        #endif

            const struct inode_operations   *i_op;
            struct super_block      *i_sb;
            struct address_space    *i_mapping;

        #ifdef CONFIG_SECURITY
            void                    *i_security;
        #endif

        * Stat data, not accessed from path walking
            unsigned long           i_ino;
        * Filesystems may only read i_nlink directly.  They shall use the
        * following functions for modification:
        *
        *    (set|clear|inc|drop)_nlink
        *    inode_(inc|dec)_link_count

            union {
                const unsigned int i_nlink;
                unsigned int __i_nlink;
            };
            dev_t                   i_rdev;
            loff_t                  i_size;
            struct timespec         i_atime;
            struct timespec         i_mtime;
            struct timespec         i_ctime;
            spinlock_t              i_lock;

        * i_blocks, i_bytes, maybe i_size

            unsigned short          i_bytes;
            unsigned int            i_blkbits;
            blkcnt_t                i_blocks;

        #ifdef __NEED_I_SIZE_ORDERED
            seqcount_t              i_size_seqcount;
        #endif

        * Misc
            unsigned long           i_state;
            struct rw_semaphore     i_rwsem;

            unsigned long           dirtied_when;

        * jiffies of first dirtying

            unsigned long           dirtied_time_when;

            struct hlist_node       i_hash;
            struct list_head        i_io_list;

        * backing dev IO list

        #ifdef CONFIG_CGROUP_WRITEBACK
            struct bdi_writeback    *i_wb;
        * the associated cgroup wb

        * foreign inode detection, see wbc_detach_inode()
            int                     i_wb_frn_winner;
            u16                     i_wb_frn_avg_time;
            u16                     i_wb_frn_history;
        #endif
            struct list_head        i_lru;

        * inode LRU list

            struct list_head        i_sb_list;
            struct list_head        i_wb_list;

        * backing dev writeback list

            union {
                struct hlist_head       i_dentry;
                struct rcu_head         i_rcu;
            };
            u64                     i_version;
            atomic_t                i_count;
            atomic_t                i_dio_count;
            atomic_t                i_writecount;
        #ifdef CONFIG_IMA
            atomic_t                i_readcount; /* struct files open RO
        #endif
            const struct file_operations    *i_fop; /* former ->i_op->default_file_ops
            struct file_lock_context        *i_flctx;
            struct address_space    i_data;
            struct list_head        i_devices;
            union {
                struct pipe_inode_info  *i_pipe;
                struct block_device     *i_bdev;
                struct cdev             *i_cdev;
                char                    *i_link;
                unsigned                i_dir_seq;
            };

            __u32                   i_generation;

        #ifdef CONFIG_FSNOTIFY
            __u32                   i_fsnotify_mask; * all events this inode cares about
            struct hlist_head       i_fsnotify_marks;
        #endif

        #if IS_ENABLED(CONFIG_FS_ENCRYPTION)
            struct fscrypt_info     *i_crypt_info;
        #endif

            void                    *i_private; * fs or device private pointer
        };
     */

    sb->s_magic = MYFS_MAGIC_NUMBER;
    
    /*
     *  MYFS_MAGIC_NUMBER must be defined in /include/api/linux/magic.h
     *  and it must be unique.
     */
    sb->s_op = &myfs_super_ops;
    /*
     *  Setting the superblock operations structures to our predefined structure
     */
	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
    root = new_inode(sb);
    /*
     *      new_inode_pseudo        - obtain an inode
     *      @sb: superblock
     *
     *      Allocates a new inode for given superblock.
     *      Inode wont be chained in superblock s_inodes list
     *      This means :
     *      - fs can't be unmount
     *      - quotas, fsnotify, writeback can't work

     struct inode *new_inode_pseudo(struct super_block *sb)
     {
        struct inode *inode = alloc_inode(sb);

        if (inode) {
            spin_lock(&inode->i_lock);
            inode->i_state = 0;
            spin_unlock(&inode->i_lock);
            INIT_LIST_HEAD(&inode->i_sb_list);
        }
        return inode;
     }
     *      new_inode       - obtain an inode
     *      @sb: superblock
     *
     *      Allocates a new inode for given superblock. The default gfp_mask
     *      for allocations related to inode->i_mapping is GFP_HIGHUSER_MOVABLE.
     *      If HIGHMEM pages are unsuitable or it is known that pages allocated
     *      for the page cache are not reclaimable or migratable,
     *      mapping_set_gfp_mask() must be called with suitable flags on the
     *      newly created inode's mapping

     struct inode *new_inode(struct super_block *sb)
     {
        struct inode *inode;

        spin_lock_prefetch(&sb->s_inode_list_lock);

        inode = new_inode_pseudo(sb);
        if (inode)
            inode_sb_list_add(inode);
        return inode;
     }
     */
    if(!root)
    {
        pr_err("Inode allocation failed.\n");
        return -ENOMEM;
    }
    /*
     *  Good kernel code handles all sorts of test cases. There could be
        instances where there isn't enough memory.
     */

    root->i_ino = 0;
    root->i_sb = sb;
    pr_info("%d is the mode\n",0777);
    root->i_atime = root->i_mtime = root->i_ctime = CURRENT_TIME;
	root->i_op = &simple_dir_inode_operations;
	root->i_fop = &simple_dir_operations;
    inode_init_owner(root,NULL,S_IFDIR | 0777);

    sb->s_root = d_make_root(root);
    /*
     *  Definition found in linux/fs/dcache.c
     struct dentry *d_make_root(struct inode *root_inode)
     {
        struct dentry *res = NULL;

        if (root_inode) {
            res = __d_alloc(root_inode->i_sb, NULL);
            if (res)
                d_instantiate(res, root_inode);
            else
                iput(root_inode);
        }
        return res;
      }
     */
    if (!sb->s_root)
    {
        pr_err("root creation failed\n");
        return -ENOMEM;
    }

    return 0;

}

void myfs_put_super(struct super_block *sb)
{
    pr_debug("myfs super block destroyed\n");
}

struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};
