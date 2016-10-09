#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <proc_file.h>

#define proc_fs_name "myprocfs"

static struct  proc_dir_entry *proc_file;
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = read,
    .open  = open,
    .release = release,
};

static int __init starter(void)
{
    proc_file = proc_create(proc_fs_name,0,NULL,&fops);
    if(proc_file == NULL)
    {
        pr_alert("Unable to initialize the proc file.\n");
        return -ENOMEM;
    }

    pr_info("Successfully created /proc/%s .\n",proc_fs_name);
    return 0;
}

static void __exit ender(void)
{
    remove_proc_entry(proc_fs_name, NULL);
}

module_init(starter);
module_exit(ender);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com");
MODULE_DESCRIPTION("This module is intended to test kernel code\
that utilizes the /proc file system.");
