#include <linux/kernel.h>
#include <linux/module.h>
#include <myfs.h>

static int __init init_my_fs(void)
{
	int err;

	err = register_filesystem(&myfs_type);
    /*
     *    A loadable module which implements a filesystem must, at load time, register that filesystem with the VFS layer.

     *    The type argument is a structure which is set up as shown in fstype.c
     */
	if (err)
		goto out;
	return 0;
out:
	return err;
}

static void __exit exit_my_fs(void)
{
	unregister_filesystem(&myfs_type);
}

MODULE_AUTHOR("Adithya Bhat");
MODULE_DESCRIPTION("My Filesystem");
MODULE_LICENSE("GPL");
module_init(init_my_fs);
module_exit(exit_my_fs);
