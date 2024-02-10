#pragma once

#include <linux/usb.h>

int fprint_probe(struct usb_interface *intf, const struct usb_device_id *id);
void fprint_disconnect(struct usb_interface *intf);
