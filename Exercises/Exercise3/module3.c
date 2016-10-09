#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>

/*
 *	The __init macro frees the function memory after executing the initialization.
 *	Very important as primary memory is very valuable. 	
 */

static int __init start(void)
{
	pr_info("freeing memory after this.\n");
	return 0;
}

static void __exit end(void)
{	
	pr_info("Standard exit procedure.\n");
}

module_init(start);
module_exit(end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_DESCRIPTION("This module is used for testing purposes.\n\
This is a part of a series of modules used to learn kernel programming.\n\
- Adithya\n");

MODULE_SUPPORTED_DEVICE("None");
