#include <linux/fs.h>

ssize_t read( struct file * filep,
    char * fname, size_t sizeval, loff_t * loffset );

int open(struct inode *inodes, struct file *files);

int release(struct inode * inodes, struct file * filep);
