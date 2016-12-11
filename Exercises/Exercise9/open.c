#include <linux/fs.h>
#include <linux/module.h>

int open(struct inode *inodes, struct file *files)
{
    try_module_get(THIS_MODULE);
    return 0;
}
