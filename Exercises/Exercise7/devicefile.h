#ifndef _TEST_DRIVER
#define _TEST_DRIVER

#include<linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
/*
 *  This header contains the necessary structures used for
    device IO operations.
 *  The file_operations structure points to the function that
    performs the device IO.
 *  Not all the functions need to be implemented by the driver.\
 *  Use the C99 way (struct x = {.key1 = value, . . .})
    over the GCC extension way (struct x = {key1:value, . . .})
    for poratbility of the driver across machines.
 */
#define BUFFER_LENGTH 256
/*
 *  The maximum length of the message that can be accessed from this
    device file.
 */

#define DEVICE_NAME "mydevice"
/*
 *  This is the name of the device file that will appear in the
    /proc/devices
 */

#define DEVICE_CLASS "this_device_is_a_class_apart"
/*
 *  This is the name of the device class used.
 NOTE:
 *  This refers to a group of devices of the same kind.
 *  Multiple devices of the same class are referred to by their minor
    numbers.
 */

int device_open(struct inode *, struct file *);
/*
 *  This function handles the device file open accesses.
 */

int device_release(struct inode *, struct file *);
/*
 *  This function performs the cleanup operations when a
    file is released.
 */

ssize_t device_read(struct file *, char *, size_t, loff_t *);
/*
 *  This function is called whenever device is being read from user
    space i.e. data is being sent from the device to the user.
    In this case it uses the copy_to_user() function to
    send the buffer string to the user and captures any errors.
 *  struct file*: A pointer to a file object (defined in linux/fs.h).
 *  char *: The pointer to the buffer to which this function
    writes the data.
 *  ssize_t: The length of the bytes requested.
 *  loff_t: The offset, if required.
 */

ssize_t device_write(struct file *, const char *, size_t, loff_t *);
/*
 *  This function is called whenever the device is being written to
    from user space i.e. data is sent to the device from the user.
    The data is copied to the message[] array in this LKM using the
    sprintf() function along with the length of the string.
 *  struct file *: A pointer to a file object.
 *  char *: The buffer to that contains the string to write
    to the device.
 *  ssize_t: The length of the array of data that is being passed
    in the const char buffer.
 *  loff_t: The offset, if required.
 */

extern int device_count;
/*
 *  This variable keeps track of the number of tasks that are
    accessing the device file.
 */

extern char message_buffer[BUFFER_LENGTH];
/*
 *  This buffer contains the bytes that are requested to perform
    operations on.
 */

extern char *buffer_ptr;
/*
 *  This pointer points to the location in the buffer to perform
    further operations.
 */

extern struct file_operations fops;
/*
 *  Setting up the operations for our device driver.
 */
#endif
