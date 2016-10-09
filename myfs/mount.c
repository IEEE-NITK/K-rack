#include <linux/fs.h>

struct dentry* myfs_mount(
    struct file_system_type *fs_type,
    int flags,
    char const *dev,
    void *data )
{
    struct dentry *const entry = mount_bdev(type, flags, dev, data, myfs_fill_sb);

    if(IS_ERR(entry))
    {
        pr_info("myfs mounting failed.\n");
    }
    else
    {
        pr_info("myfs mounting successful.\n");
    }
    return entry;
}
