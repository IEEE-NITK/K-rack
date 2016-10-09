#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/moduleparam.h>

/*
 *	To use the command line arguements use the following steps:
 *	1. #include the <linux/moduleparam.h> header file.
 *	2. Declare command line argument variables as global variables.
 *	3. Use module_param(variable_name, datatype, permissions ) macro to 
	   setup each variable.
 *	4. Use module_param_array() and module_param_string() macros 
	   for arrays and strings respectively.
		   
	NOTE:
 *	module_param ( var-name , var-type , perm )
 	* The first param is the parameters name
 	* The second param is it's data type
 	* The final argument is the permissions bits, 
 	  for exposing parameters in sysfs (if non-zero) at a later stage.
 		  
 *	module_param_array ( var-name , var-type , array-count-holder , perm );
	* The first param is the parameter's (in this case the array's) name
	* The second param is the data type of the elements of the array
	* The third argument is a pointer to the variable that will store the number 
	  of elements of the array initialized by the user at module loading time
 	* The fourth argument is the permission bits
 */



/*
 * The variables are static inorder to restrict the variable scope to the file.
 * This is needed to prevent name collisions when used with the huge kernel source.
 * The variables can be initialised for "AUTODETECTION".
 */

static int 	intparam 		= 0;
static char *stringparam 	= NULL;
static int 	arraycount 		= 0;
static int 	arrayparam[10];

/*
	* Actual setting up of the variables is done by the macros shown below.
*/

module_param 		( intparam		, int 	, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
module_param 		( stringparam	, charp , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP	);
module_param_array	( arrayparam	, int	, &arraycount , 	  S_IRUSR | S_IWUSR );

/*
 * A description for each of the parameter used.
 * Optional, but is useful for debugging and documentational purposes.
 */

MODULE_PARM_DESC ( intparam 	, "Integer parameter for my module.\n"			);
MODULE_PARM_DESC ( stringparam 	, "String parameter for my module.\n"			);
MODULE_PARM_DESC ( arrayparam	, "Array of Integers parameter for my module.\n");

/*
 * Module start stub is located here.
 */

static int __init start(void)
{
	pr_info ("Int parameter's value is %d\n."	,intparam	);
	pr_info ("String parameter's value is %s\n.",stringparam);
	
	if ( arraycount != 0 ) {
		int i;
		
		for( i = 0; i < arraycount; i++ ) {
			pr_info("Array %d value is %d.",i+1,arrayparam[i]);
		}
		
	}
	
	pr_info ("Loaded the module successfully.\n");
	
	return 0;
}

static void __exit end(void)
{	
	pr_info("Removing the module successfully.\n");
}

/*
 * Registering the handlers here.
 */

module_init(start);
module_exit(end);

/*
 * Used to prevent the kernel tainted warning alerts.
 */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_DESCRIPTION("This module is used for testing purposes.\n\
This module explains the usage of command line arguments for a kernel module.\n\
- Adithya\n");

MODULE_SUPPORTED_DEVICE("None");
