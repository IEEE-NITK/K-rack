#include <linux/fs.h>
#include <asm/uaccess.h>

ssize_t read( struct file * filep,
    char * fname, size_t sizeval, loff_t * loffset )
{
    static int finished = 0;
    int processed_bytes = 0;
    /*
     *  processed_bytes variable keeps track of the
        number of bytes read successfully.
     *  It is also the return value of the read
        function.
     *  sizeval is the number of bytes to be read.
     *  Loop until all the requested bytes are provided
        or we run out of content to provide.
     *  put_user is a macro that takes a value and copies
        it to a destination in the user space.
     */

    char message[] = "Hello /proc World.\n";
    char *buffer_ptr = message;
    if(finished)
    {
        finished = 0;
        return 0;
    }
    finished = 1;
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
