#include<linux/kernel.h>
#include<linux/module.h>

/*

 *	kernel.h contains the alert values.

 * 	KERN_EMERG	:"\0010" Used for emergency messages, usually those that precede a crash.
 *	KERN_ALERT	:"\0011" A situation requiring immediate action.
 *	KERN_CRIT	:"\0012" Critical conditions, often related to serious hardware or software failures.
 *	KERN_ERR	:"\0013" Used to report error conditions;
				  device drivers often use KERN_ERR to report hardware difficulties.
 * 	KERN_WARNING:"\0014" Warnings about problematic situations that do not, in themselves,
				  create serious problems with the system.
 *	KERN_NOTICE	:"\0015" Situations that are normal, but still worthy of note.
				  A number of security-related conditions are reported at this level.
 * 	KERN_INFO	:"\0016" Informational messages. Many drivers print information
                  about the hardware they find at startup time at this level.
 *	KERN_DEBUG	:"\0017" Used for debugging messages.
 *	KERN_DEFAULT:"\001d" Default logging values.

 *	More info can be found at linux/kern_levels.h

 */

int start(void)
{
	/*
		Usage: printk is now deprecated.
		pr_info, pr_dbg , . . . etc called pr_** are now preferred.
		More info can be found at linux/printk.h
	 */
	printk(KERN_INFO"Module 1 loaded.\n");
    /*
     *  printk("<6>MOdule .. .")
     */
	return 0;
	/*
	 *	A negative return value results in the following message:

	 * 	"insmod: ERROR: could not insert module ./####.ko: Operation not permitted"

	 */
}

void end(void)
{
	printk(KERN_INFO"Module one is removed.\n");
}

/*
 *	Instead of module_init and module_exit, init_module() and
 *	cleanup_module can be used.
 *	However, their usage is deprecated from kernel 2.6 onwards.
 */
module_init(start);
module_exit(end);
