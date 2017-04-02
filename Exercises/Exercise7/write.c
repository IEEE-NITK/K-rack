#include<devicefile.h>

ssize_t device_write( struct file * filep, const char *buffer,
    size_t len, loff_t *offset)
{
    int processed_bytes=0,count=0;
    /*
        message_buffer is the char array that is written to 
        the device file i.e /dev/mynull
    */
    memset(message_buffer,0,256);
    while(len>0)
    {
	message_buffer[processed_bytes++]=buffer[count++];
	len--;    
    }    

    return processed_bytes;
    
}
