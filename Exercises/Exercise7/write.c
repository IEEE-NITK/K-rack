#include<devicefile.h>

ssize_t device_write(
    struct file * filep,
    const char *buffer,
    size_t len,
    loff_t *offset)
{
    /*
     *  Currently, the write operation is not supported.
        I am still learning :P.
     */
    pr_alert("Sorry, this operation is not supported yet.\n");
    return -EINVAL;
}
