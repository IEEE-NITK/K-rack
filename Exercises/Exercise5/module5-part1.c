#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

/*
 *	This module demonstrates spanning of a module across multiple source files.
 */

static int __init start(void){
	pr_info("Module loaded.\n");
	return 0;
}


module_init(start);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("This module is used for testing purposes.\n\
This module explains the usage of command line arguments for a kernel module.\n\
- Adithya\n");

