#include<linux/kernel.h>
#include<linux/module.h>

/*
 *	The use of pr_** macros is preferred for dynamic debug support.
 *	printk(KERN_<LEVEL>"..") is equivalent to pr_<LEVEL>("..")	
 */


int startfun(void)
{
	pr_info("Testing.\n");
	return 0;
}

void endfunc(void){
	pr_info("Test complete.\n");
}

module_init(startfun);
module_exit(endfunc);
