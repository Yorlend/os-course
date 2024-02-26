#pragma once

#include <bits/types/struct_timespec.h>
#include <stddef.h>

#define PE_OK    0
#define PE_NODEV 1
#define PE_BUSY  2
#define PE_AGAIN 3
#define PE_CANCL 4

#define SIGNATURE_IMAGE_WIDTH  504
#define SIGNATURE_IMAGE_HEIGHT 480
#define SIGNATURE_IMAGE_SIZE ((SIGNATURE_IMAGE_WIDTH) * (SIGNATURE_IMAGE_HEIGHT))

#pragma pack(push, 1)

struct signature {
    char image_data[SIGNATURE_IMAGE_SIZE];
};

#pragma pack(pop)

/**
 * Scans fingerprint signature into signature struct.
 *
 * @returns PE_OK upon success,
 *          PE_NODEV if kernel module not loaded,
 *          PE_BUSY if device is busy,
 *          PE_AGAIN if timeout,
 *          PE_CANCL if cancelled
 */
int scan_signature(struct signature *signature, struct timespec *timeout);

int scan_timed(struct signature *signature, unsigned long long* time_ns, size_t buffer_size);
