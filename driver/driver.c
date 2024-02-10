#include <linux/module.h>
#include <linux/usb.h>
#include "constants.h"
#include "proc.h"
#include "entry.h"

MODULE_AUTHOR("Klim Kornienko IU7-71");
MODULE_LICENSE("GPL");

#define VENDOR_ID 0x27c6
#define PRODUCT_ID 0x5125

static struct usb_device_id fprint_id_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    { 0 },
};
MODULE_DEVICE_TABLE(usb, fprint_id_table);

static struct usb_driver fprint_driver = {
    .name = "fprint",
    .probe = fprint_probe,
    .disconnect = fprint_disconnect,
    .id_table = fprint_id_table,
};

int __init init_driver(void)
{
    printk(KERN_INFO LOG_PREFIX "================================================\n");
    printk(KERN_INFO LOG_PREFIX "loading fingerprint driver\n");
    int result = usb_register(&fprint_driver);
    if (result < 0) {
        printk(KERN_ERR LOG_PREFIX "usb_register failed\n");
        return result;
    }

    result = register_fprint_proc_file();
    if (result < 0) {
        printk(KERN_ERR LOG_PREFIX "failed to register proc file\n");
        usb_deregister(&fprint_driver);
        return result;
    }

    return 0;
}

void __exit cleanup_driver(void)
{
    unregister_fprint_proc_file();
    usb_deregister(&fprint_driver);
    printk(KERN_INFO LOG_PREFIX "fingerprint driver unloaded\n");
    printk(KERN_INFO LOG_PREFIX "================================================\n");
}

module_init(init_driver);
module_exit(cleanup_driver);
