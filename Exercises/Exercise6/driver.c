#include <devicefile.h>

/*
 *  Registering module initialization and cleanup hadnlers here.
 */

char    message_buffer[BUFFER_LENGTH];
char*   buffer_ptr;
int     Major_num            = 0;
int     device_count         = 0;
struct  file_operations fops = {
        .read                = device_read,
        .write               = device_write,
        .open                = device_open,
        .release             = device_release
};

static int __init starter(void)
{
    Major_num = register_chrdev(0 , DEVICE_NAME , &fops );
    /*
     *  Register a major number for character devices.
     *  int register_chrdev ( unsigned int major, const char *name, const
        struct file_operations *fops);
        major: major device number or 0 for dynamic allocation
        name: name of this range of devices
        fops: file operations associated with this devices
     *  If major equals zero, this function will dynamically allocate a major
        and return its number.
     *  If major is greater than zero, this function will attempt to reserve a
        device with the given major number and will return zero on success.
     *  It returns a negative error number on failure.
     *  This function registers a range of 256 minor numbers. The first minor
        number is 0.
     */
    if ( Major_num < 0 ){
        pr_alert("Registration of the driver failed with %d.\n",Major_num);
        return Major_num;
    }

    /*
     *  Successful Character Device registration.
     */

    pr_info("Registered the driver with the major number as %d.\n",Major_num);
    return 0;
}

static void __exit ender(void)
{
    unregister_chrdev(Major_num, DEVICE_NAME);
    /*
     *  Unregisters a character device and frees the major number.
        This function asserts that the device is not used by any
        process anymore.
     */
}

module_init(starter);
module_exit(ender);

MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("I intend to write a character class driver with this module.\n\
Purely for learning purposes.\n");
MODULE_SUPPORTED_DEVICE("USB-Coming Soon");
