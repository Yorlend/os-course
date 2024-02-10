#pragma once

#include <linux/usb.h>

struct fprint_drv_data {
    struct urb* urb;

    void* transfer_buffer;
#define TRANSFER_BUFFER_LENGTH 4096
};

void completion_handler(struct urb* urb);
