#include <linux/fs.h>

ssize_t read( struct file * filep,
    char * fname, size_t sizeval, loff_t * loffset );

/*
 *  Modified it so that it can now show the per cpu frequency
 *  instead of random junk.
 */

int open(struct inode *inodes, struct file *files);
/*
 *  An open file operation handler.
 */

int release(struct inode * inodes, struct file * filep);
/*
 *  A file operation handler that can handle the file close calls.
 */
