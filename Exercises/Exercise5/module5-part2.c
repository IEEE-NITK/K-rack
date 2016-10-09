#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

/*
 *	This module demonstrates spanning of a module across multiple source files.
 */

static void __exit end(void){
	pr_info("Module unloaded.\n");
}

module_exit(end);

MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_SUPPORTED_DEVICE("None");
