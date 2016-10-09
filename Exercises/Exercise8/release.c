#include <linux/fs.h>
#include <linux/module.h>

int release(struct inode * inodes, struct file * filep)
{
    module_put(THIS_MODULE);
    return 0;
}
