#include <linux/fs.h>

struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name  = "myfs",
    .mount = myfs_mount,
    .kill_sb = kill_block_super,
    .fs_flags = FS_REQUIRES_DEV,
};
