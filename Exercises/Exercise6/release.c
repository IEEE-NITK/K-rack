#include<devicefile.h>

int device_release(struct inode * inodes, struct file * filep)
{
    device_count--;
    /*
     *  release the lock on the device file after the operation
        is complete.
     */
    module_put(THIS_MODULE);
    /*
     *  The module_put macro decreases the kernel's internal usage
        count of the current module by one.
     */
    return 0;
}
