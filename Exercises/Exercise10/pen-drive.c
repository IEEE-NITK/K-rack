#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

static void __exit pdrive_exit(void);
static int __init pdrive_init(void);

module_init(pdrive_init);
module_exit(pdrive_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adithya Bhat <dth.bht@gmail.com>");
MODULE_DESCRIPTION("USB Driver in the making");

/*
 *  Some module initializations.
 */

static int the_pen_prober (  struct          usb_interface *interface,
                            const struct    usb_device_id *id )
{
    /*
     *  What the device should do in the system kernel when
     *  a new device is detected. Kernel initialization for
     *  the device is done here.
     *  The probe function is only called only when no driver
     *  is handling the device.
     *  In other words, if another driver is responsible for
     *  the device, then the probe function is not called.
     */
    pr_info ("Pen drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}

static void the_pen_disconnector ( struct usb_interface *interface )
{
    /*
     *  What the driver should do in the kernel when it's
     *  device is removed from the system.
     *  Kernel memory cleanup for the device is done here.
     */
    pr_info("Pen drive removed\n");
}

static struct usb_device_id table_of_pen_drive_ids[] =
{
    /*
     *  USB_DEVICE Macro takes as argument, the vendor in hexadecimal
     *  followed by the id in hexadecimal
     */
    {
        USB_DEVICE(0x930,0x6545)
    },
    {} /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, table_of_pen_drive_ids);


/*
 *  The global structure containing the USB Driver's Core
 *  functionalities defined, used during the registration.
 */

static struct usb_driver my_driver =
{
    .name = "The super awesome K-Rack pen drive",
    .id_table = table_of_pen_drive_ids,
    .probe = the_pen_prober,
    .disconnect = the_pen_disconnector,
};

static int __init pdrive_init(void)
{
    /*
     *  On inserting the module, register itself to detect
     *  any device with the given id.
     */
    int ret;
    pr_info ("Registering module\n");
    ret = usb_register(&my_driver);
    if (ret != 0 ){
        pr_alert ("Registration failed.\n");
        return ret;
    }
    pr_info ("Registration successful\n");
    return ret;
}

/*
 *  Upon exit, one must always cleanup.
 *  Freeing kernel memory is very important.
 */

static void __exit pdrive_exit(void)
{
    usb_deregister(&my_driver);
}
