#include<devicefile.h>

int device_open(struct inode * file, struct file * filep)
{
    static int counter = 0;
    /*
     *  counter keeps track of the number of read operations
     *  performed.
     *  If the value of the device_count variable is greater
        than zero, consider the device to be busy, and leave
        goddamn device alone.
     *  If the device_count is zero, lock it by making it
        non zero.
     */

    if ( device_count )
    {
        return -EBUSY;
    }

    device_count++;
    /*
     *  Set the contents of the file with this operation.
     *  The read function reads from the buffer_ptr, hence
        point the poor chap in the right direction.
     */
    sprintf ( message_buffer , "This device has performed %d \
    read operations.\n" , counter++ );
    buffer_ptr = message_buffer;

    try_module_get(THIS_MODULE);
    /*
     *  Before calling into a module code, you should call
        try_module_get() on that module: if it fails, then
        the module is being removed and you should act as
        if it wasn't there. Otherwise, you can safely
        enter the module, and call module_put() when you're
        finished.
     *  This macro increases the kernel's internal count of
        the number of tasks using the module.
     *  THIS_MODULE is a macro refers to the current module.
     */
    return 0;
}
