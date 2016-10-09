#include<devicefile.h>

ssize_t device_read
  ( struct file * filep,
    char * fname,
    size_t sizeval,
    loff_t * loffset )
{
    int processed_bytes = 0;
    /*
     *  processed_bytes variable keeps track of the
        number of bytes read successfully.
     *  It is also the return value of the read
        function.
     */

    if(*buffer_ptr == 0)
    {
        /*
         *  If the message to displayed is not ready
            back off.
         */
        return 0;
    }

    /*
     *  sizeval is the number of bytes to be read.
     *  Loop until all the requested bytes are provided
        or we run out of content to provide.
     *  put_user is a macro that takes a value and copies
        it to a destination in the user space.
     */
    while( sizeval && *buffer_ptr )
    {
        put_user(*(buffer_ptr++),fname++);
        sizeval--;
        processed_bytes++;
    }

    /*
     *  (NOTE: By popular convention the number of processed
        bytes are returned. However, it is not mandatory to
        do so).
     */
    return processed_bytes;
}
