#include <linux/kernel.h>
#include <linux/module.h>
#include <myfs.h>

static int __init init_ext2_fs(void)
{
	int err;

	err = register_filesystem(&myfs_type);
	if (err)
		goto out;
	return 0;
out:
	return err;
}

static void __exit exit_ext2_fs(void)
{
	unregister_filesystem(&myfs_type);
}

MODULE_AUTHOR("Adithya Bhat");
MODULE_DESCRIPTION("My Filesystem");
MODULE_LICENSE("GPL");
module_init(init_ext2_fs);
module_exit(exit_ext2_fs);
