#include <linux/module.h>
#include "constants.h"
#include "fprintdrv.h"
#include "entry.h"

static struct usb_ctrlrequest req;

void completion_handler(struct urb* urb)
{
    printk(KERN_INFO LOG_PREFIX "fingerprint usb transfer completed\n");
    printk(KERN_INFO LOG_PREFIX " stream_id = %d\n", urb->stream_id);
    printk(KERN_INFO LOG_PREFIX " status = %d\n", urb->status);
    printk(KERN_INFO LOG_PREFIX " actual length = %d\n", urb->actual_length);
}

int initialize_fprint_endpoint(struct usb_interface *intf, struct usb_endpoint_descriptor *endpoint)
{
    struct fprint_drv_data *drv_data = kzalloc(sizeof(struct fprint_drv_data), GFP_KERNEL);
    if (!drv_data) {
        printk(KERN_ERR LOG_PREFIX "failed to allocate driver data structure\n");
        return -ENOMEM;
    }
    printk(KERN_INFO LOG_PREFIX "allocated driver data structure\n");

    drv_data->transfer_buffer = kzalloc(TRANSFER_BUFFER_LENGTH, GFP_KERNEL);
    if (!drv_data->transfer_buffer) {
        printk(KERN_ERR LOG_PREFIX "failed to allocate transfer buffer\n");
        goto cleanup;
    }
    printk(KERN_INFO LOG_PREFIX "allocated transfer buffer\n");

    // drv_data->urb = usb_alloc_urb(0, GFP_KERNEL);
    // if (!drv_data->urb) {
    //     printk(KERN_ERR LOG_PREFIX "failed to allocate URB\n");
    //     goto cleanup1;
    // }
    // printk(KERN_INFO LOG_PREFIX "allocated URB\n");

    struct usb_device* usbdev = interface_to_usbdev(intf);
    dev_set_drvdata(&usbdev->dev, drv_data);

    // int pipe = usb_sndctrlpipe(usbdev, endpoint->bEndpointAddress);
    char data[100];

    // fill control urb
    int ret = usb_control_msg_send(
        usbdev,
        endpoint->bEndpointAddress,
        USB_REQ_GET_STATUS,
        cpu_to_le16(0xa3),
        0,
        0,
        data,
        4,
        1000,
        GFP_KERNEL
    );
    printk(KERN_INFO LOG_PREFIX "filled control URB\n");

    // submit urb
    if (ret == 0) {
        printk(KERN_INFO LOG_PREFIX "submitted URB\n");
    } else {
        printk(KERN_ERR LOG_PREFIX "failed to submit URB ret = %d\n", ret);
        goto cleanup2;
    }

    return 0;

cleanup2:
    // usb_free_urb(drv_data->urb);

cleanup1:
    kfree(drv_data->transfer_buffer);

cleanup:
    kfree(drv_data);
    return -EIO;
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

    if (iface->desc.bNumEndpoints != 3) {
        printk(KERN_INFO LOG_PREFIX "skipping this interface\n");
        return 0;
    }

    // work with interface number 0 and endpoint number 0
    if (iface->desc.bInterfaceNumber != 0) {
        printk(KERN_INFO LOG_PREFIX "skipping this interface\n");
        return 0;
    }

    struct usb_endpoint_descriptor *endpoint = &iface->endpoint[0].desc;
    if (endpoint->bEndpointAddress != 0x82) {
        printk(KERN_WARNING LOG_PREFIX "endpoint[1] must have adress 0x81\n");
        return 0;
    }

    int res = initialize_fprint_endpoint(intf, endpoint);
    if (res != 0) {
        printk(KERN_ERR LOG_PREFIX "failed to initialize endpoint\n");
    }
    return res;
}

void fprint_disconnect(struct usb_interface *intf)
{
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface disconnected\n");

    struct usb_host_interface *iface = intf->cur_altsetting;
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface number: %d\n", iface->desc.bInterfaceNumber);
    printk(KERN_INFO LOG_PREFIX "fingerprint usb interface string: %d\n", iface->desc.iInterface);

    // free allocated structures here

    // work with interface number 0 and endpoint number 0
    if (iface->desc.bInterfaceNumber != 0) {
        printk(KERN_INFO LOG_PREFIX "skipping this interface\n");
        return;
    }

    struct usb_endpoint_descriptor *endpoint = &iface->endpoint[0].desc;
    if (endpoint->bEndpointAddress != 0x82) {
        printk(KERN_WARNING LOG_PREFIX "endpoint[1] must have adress 0x81\n");
        return;
    }

    struct usb_device* usbdev = interface_to_usbdev(intf);
    struct fprint_drv_data* drvdata = dev_get_drvdata(&usbdev->dev);
    if (drvdata != NULL) {
        // usb_free_urb(drvdata->urb);
        kfree(drvdata->transfer_buffer);
        kfree(drvdata);
        printk(KERN_INFO LOG_PREFIX "driver data free'd\n");
    }
}
