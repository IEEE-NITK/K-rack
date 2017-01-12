#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <asm/page.h> 		/* PAGE_SIZE */
#include <linux/fs.h>     	/* This is where libfs stuff is declared */
#include <asm/atomic.h>
#include <asm/uaccess.h>	/* copy_to_user */

MODULE_LICENSE("GPL");

const struct inode_operations lfs_dir_inode_ops;
const struct super_operations lfs_s_ops;
const struct inode_operations lfs_file_inode_operations;
const struct file_operations  lfs_file_operations;
unsigned long get_unmapped(struct file *, unsigned long , unsigned long , unsigned long , unsigned long);
struct inode *lfs_make_inode(struct super_block*,const struct inode*, int, dev_t);
int lfs_make_node(struct inode*, struct dentry *,umode_t, dev_t);
int lfs_make_directory(struct inode *,struct dentry *, umode_t);
int lfs_create(struct inode *,struct dentry *,umode_t,bool);
int lfs_fill_super (struct super_block *, void *, int);

const struct inode_operations lfs_dir_inode_ops = {
	.create = lfs_create,
	.lookup = simple_lookup,
	.link = simple_link,
	.unlink = simple_unlink,
	.mkdir = lfs_make_directory,
	.rmdir = simple_rmdir,
	.mknod = lfs_make_node,
	.rename = simple_rename,
};

const struct super_operations lfs_s_ops = {
	.statfs = simple_statfs,
	.drop_inode = generic_delete_inode,
	.show_options = generic_show_options,
};

const struct inode_operations lfs_file_inode_operations = {
	.setattr	= simple_setattr,
	.getattr	= simple_getattr,
};

const struct file_operations lfs_file_operations = {
	.read_iter	= generic_file_read_iter,
	.write_iter = generic_file_write_iter,
	.mmap		= generic_file_mmap,
	.fsync		= noop_fsync,
	.splice_read= generic_file_splice_read,
	.splice_write	= iter_file_splice_write,
	.llseek		= generic_file_llseek,
	.get_unmapped_area = get_unmapped,
};



#define LFS_MAGIC 0x19980122

unsigned long get_unmapped(struct file *file, unsigned long addr, unsigned long len, unsigned long pgoff, unsigned long flags)
{
	return current->mm->get_unmapped_area(file,addr,len,pgoff,flags);
}

struct inode *lfs_make_inode(struct super_block *sb, const struct inode *dir, int mode, dev_t dev)
{
	struct inode *ret = new_inode(sb);
	if (ret) {
		ret->i_ino  = get_next_ino();
		inode_init_owner(ret,dir,mode);
		ret->i_atime = ret->i_mtime = ret->i_ctime = CURRENT_TIME;
		switch(mode & S_IFMT){
		case S_IFREG:
			ret->i_op = &lfs_file_inode_operations;
			ret->i_fop = &lfs_file_operations;
			break;
			
		case S_IFDIR:
			ret->i_op = &simple_dir_inode_operations;
			ret->i_fop= &simple_dir_operations;
		}
	}
	return ret;
}

int lfs_make_node(struct inode *dir, struct dentry *dentry,umode_t mode, dev_t dev)
{
	struct inode *inode = lfs_make_inode(dir->i_sb,dir,mode,dev);
	int err = -ENOSPC;
	
	if(inode) {
		d_instantiate(dentry,inode);
		dget(dentry);
		err = 0;
		dir->i_mtime = dir->i_ctime = CURRENT_TIME;
	}
	return err;
}

int lfs_make_directory(struct inode *dir,struct dentry *dentry, umode_t mode)
{
	int retval = lfs_make_node(dir,dentry,mode|S_IFDIR, 0);
	if(!retval)
	{
		inc_nlink(dir);
	}
	return retval;
}

int lfs_create(struct inode *dir, struct dentry *dentry, umode_t mode,bool excl)
{
	return lfs_make_node(dir,dentry, mode|S_IFREG,0);
}



/*
 * "Fill" a superblock with mundane stuff.
 */
int lfs_fill_super (struct super_block *sb, void *data, int silent)
{
	struct inode *root;
	//struct dentry *root_dentry;
/*
 * Basic parameters.
 */
	sb->s_blocksize = PAGE_SIZE;
	sb->s_blocksize_bits = PAGE_SHIFT;
	sb->s_magic = LFS_MAGIC;
	sb->s_op = &lfs_s_ops;
/*
 * We need to conjure up an inode to represent the root directory
 * of this filesystem.  Its operations all come from libfs, so we
 * don't have to mess with actually *doing* things inside this
 * directory.
 */
	root = lfs_make_inode (sb, NULL, S_IFDIR | 0777 , 0);
	//root = new_inode(sb);
	if (! root)
		goto out;
	
    sb->s_root = d_make_root(root);

	if(!sb->s_root)
	out:
		return -ENOMEM;	
	
//	lfs_create_files (sb, sb->s_root);
	return 0;
	
}

struct dentry *lfs_get_super(struct file_system_type *fst,
		int flags, const char *devname, void *data)
{
	return mount_bdev(fst, flags, devname, data, lfs_fill_super);
}

struct file_system_type lfs_type = {
	.owner 		= THIS_MODULE,
	.name		= "lfs",
	.mount		= lfs_get_super,
	.kill_sb	= kill_litter_super,
	.fs_flags 	= FS_USERNS_MOUNT,
};

int __init lfs_init(void)
{
	return register_filesystem(&lfs_type);
}

void __exit lfs_exit(void)
{
	unregister_filesystem(&lfs_type);
}

module_init(lfs_init);
module_exit(lfs_exit);
