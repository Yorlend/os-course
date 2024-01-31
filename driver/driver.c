#include <linux/module.h>
#include <linux/usb.h>
#include "fprintdrv.h"

MODULE_AUTHOR("Klim Kornienko IU7-71");
MODULE_LICENSE("GPL");

#define LOG_PREFIX "fprint: "

#define VENDOR_ID 0x27c6
#define PRODUCT_ID 0x5125

static struct usb_device_id fprint_id_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    { 0 },
};
MODULE_DEVICE_TABLE(usb, fprint_id_table);

int fprint_probe(struct usb_interface *intf, const struct usb_device_id *id);
void fprint_disconnect(struct usb_interface *intf);

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

    return 0;
}

void __exit cleanup_driver(void)
{
    usb_deregister(&fprint_driver);
    printk(KERN_INFO LOG_PREFIX "fingerprint driver unloaded\n");
    printk(KERN_INFO LOG_PREFIX "================================================\n");
}

int fprint_probe(struct usb_interface *intf, const struct usb_device_id *id)
{
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface probed\n");

    struct usb_host_interface *iface = intf->cur_altsetting;
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface number: %d\n", iface->desc.bInterfaceNumber);
    if (iface->string != NULL) {
        printk(KERN_INFO LOG_PREFIX "fingerprint usb interface string: %s\n", iface->string);
    } else {
        printk(KERN_WARNING LOG_PREFIX "fingerprint usb interface string is NULL\n");
    }

    // display all information about interface
    for (int i = 0; i < iface->desc.bNumEndpoints; i++) {
        struct usb_endpoint_descriptor *endpoint = &iface->endpoint[i].desc;
        printk(KERN_INFO LOG_PREFIX "fingerprint usb interface endpoint[%d] address: %d\n", i, endpoint->bEndpointAddress);
    }

    // work with interface number 1 and endpoint number 1
    if (iface->desc.bInterfaceNumber != 1) {
        printk(KERN_INFO LOG_PREFIX "skipping this interface\n");
        return 0;
    }

    struct usb_endpoint_descriptor *endpoint = &iface->endpoint[1].desc;
    if (endpoint->bEndpointAddress != 0x81) {
        printk(KERN_WARNING LOG_PREFIX "endpoint[1] must have adress 0x81\n");
        return 0;
    }

    struct fprint_drv_data *drv_data = kzalloc(sizeof(struct fprint_drv_data), GFP_KERNEL);
    if (!drv_data) {
        printk(KERN_ERR LOG_PREFIX "failed to allocate driver data structure\n");
        return -ENOMEM;
    }
    printk(KERN_INFO LOG_PREFIX "allocated driver data structure\n");

    struct usb_device* usbdev = interface_to_usbdev(intf);
    const void* drvdata = dev_get_drvdata(&usbdev->dev);
    if (drvdata != NULL) {
        printk(KERN_ERR LOG_PREFIX "drvdata is not NULL\n");
    }

    dev_set_drvdata(&usbdev->dev, drv_data);

    // kfree(drv_data);
    return 0;
}

void fprint_disconnect(struct usb_interface *intf)
{
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface disconnected\n");

    struct usb_host_interface *iface = intf->cur_altsetting;
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface number: %d\n", iface->desc.bInterfaceNumber);
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface string: %d\n", iface->desc.iInterface);

    // free allocated structures here

    // work with interface number 1 and endpoint number 1
    if (iface->desc.bInterfaceNumber != 1) {
        printk(KERN_INFO LOG_PREFIX "skipping this interface\n");
        return;
    }

    struct usb_endpoint_descriptor *endpoint = &iface->endpoint[1].desc;
    if (endpoint->bEndpointAddress != 0x81) {
        printk(KERN_WARNING LOG_PREFIX "endpoint[1] must have adress 0x81\n");
        return;
    }

    struct usb_device* usbdev = interface_to_usbdev(intf);
    const void* drvdata = dev_get_drvdata(&usbdev->dev);
    if (drvdata == NULL) {
        printk(KERN_ERR LOG_PREFIX "driver data is NULL\n");
    } else {
        printk(KERN_INFO LOG_PREFIX "driver data free'd\n");
        kfree(drvdata);
    }
}

module_init(init_driver);
module_exit(cleanup_driver);
