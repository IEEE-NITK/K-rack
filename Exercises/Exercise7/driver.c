#include <devicefile.h>
#include <linux/cdev.h>
#include <linux/device.h>

int device_count            = 0;
struct file_operations fops = {
    .read                   = device_read,
    .write                  = device_write,
    .open                   = device_open,
    .release                = device_release
};

/*
 *  dev_t is the type used to represent device numbers within
    the kernel.
 *  The kernel uses structures of type struct cdev to represent
    char devices internally.
 *  A class is a higher-level view of a device that abstracts
    out low-level implementation details. Classes allow user
    space to work with devices based on what they do, rather
    than how they are connected or how they work.
 */
static dev_t  devid;
static struct cdev char_device;
static struct class *devcl;
char          message_buffer[BUFFER_LENGTH];
char*         buffer_ptr;

static int __init starter(void)
{
    int ret = alloc_chrdev_region(&devid, 0, 1, DEVICE_NAME);
    /*
     *  int alloc_chrdev_region (	dev_t * dev, unsigned baseminor,
 	    unsigned count,const char * name) registers a range of char
        device numbers.
     *  dev: output parameter for first assigned number.
     *  baseminor: first of the requested range of minor numbers.
     *  count: the number of minor numbers required.
     *  name: the name of the associated device or driver.
     *  (NOTE:The major number will be chosen dynamically, and
        returned (along with the first minor number) in dev.)
     *  It returns zero on success or a negative error code on
        failure.
     */
    if( ret < 0 )
    {
        pr_info("Device number allocation failed with %d.\n", ret);
        return -1;
    }

    devcl = class_create(THIS_MODULE, DEVICE_CLASS);
    /*
     *  struct class *class_create(struct module *owner,const char *name);
     *  owner: pointer to the module that is to “own” this struct class.
     *  name: pointer to a string for the name of this class.
     *  This is used to create a struct class pointer that can then be
        used in calls to class_device_create.
     */
    if(devcl == NULL)
    {
        unregister_chrdev_region(devid,1);
        return -1;
    }

    /*
     *  struct device *device_create(struct class *class,struct
     *  device *parent,dev_t devt,void * drvdata,const char * fmt, ...);
     *  class: pointer to the struct class that this device should be
        registered to.
     *  parent: pointer to the parent struct device of this new device,
        if any.
     *  devt: the dev_t for the char device to be added.
     *  drvdata: the data to be added to the device for callbacks.
     *  fmt: string for the device's name.
     *  ...: variable arguments.
     *  This function can be used by char device classes. A struct device
        will be created in sysfs, registered to the specified class.
     *  A “dev” file will be created, showing the dev_t for the device,
        if the dev_t is not 0,0.
     *  If a pointer to a parent struct device is passed in, the newly created
        struct device will be a child of that device in sysfs.
     *  The pointer to the struct device will be returned from the call.
     *  Any further sysfs files that might be required can be created using
        this pointer.
     *  It returns struct device pointer on success, or ERR_PTR on error.
     *  (NOTE:the struct class passed to this function must have previously
        been created with a call to class_create.)
     */
    if( device_create(devcl,NULL,devid,NULL,"mynull") == NULL )
    {
        class_destroy(devcl);
        unregister_chrdev_region(devid,1);
        return -1;
    }

    cdev_init(&char_device, &fops);
    /*
     *  void cdev_init(struct cdev *cdev,const struct file_operations *fops);
     *  cdev: the structure to initialize.
     *  fops: the file_operations for this device.
     *  It initializes cdev, remembering fops, making it ready to add to
        the system with cdev_add.
     *  int cdev_add(struct cdev *p,dev_t dev,unsigned count);
     *  p: the cdev structure for the device.
     *  dev: the first device number for which this device is responsible.
     *  count: the number of consecutive minor numbers corresponding to
        this device.
     *  cdev_add adds the device represented by p to the system, making it live
        immediately.
     *  A negative error code is returned on failure.
     */
    if(cdev_add(&char_device , devid, 1) == -1)
    {
        device_destroy(devcl,devid);
        class_destroy(devcl);
        unregister_chrdev_region(devid,1);
        return -1;
    }
    pr_info("Module loaded Successfully.\n");
    return 0;
}

/*
 *  cdev_del removes a cdev structure from the system,
    possibly freeing the structure itself.
 *  void device_destroy(struct class *class, dev_t devt)
    unregisters and cleans up a device that was created
    with a call to device_create.
 *  void class_destroy(struct class *cls) destroys a
    struct class structure.
    (NOTE: the pointer to be destroyed must have been
    created with a call to class_create)
 *  void unregister_chrdev_region(dev_t from,unsigned count)
    will unregister a range of count device numbers, starting
    with from.
    (NOTE:The caller should normally be the one who allocated
    those numbers in the first place.)
 */
static void __exit ender(void)
{
    cdev_del (&char_device);
    device_destroy(devcl,devid);
    class_destroy(devcl);
    unregister_chrdev_region(devid,1);
    pr_info("Successfully unregistered.\n");
}

/*
 *  Registering module initialization and cleanup hadnlers here.
 *  The usual bleh code goes here.
 */

module_init(starter);
module_exit(ender);

MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("I intend to write a character class \
driver with this module.\n\
Purely for learning purposes.\n");
MODULE_SUPPORTED_DEVICE("USB-Coming Soon");
