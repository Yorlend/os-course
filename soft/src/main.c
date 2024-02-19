#include <stdio.h>
#include "fprint.h"

void test_timed(void) {
    unsigned long long delta_time = 0;

    int res = scan_timed(NULL, &delta_time, 65536);
    if (res == PE_OK) {
        printf("delta time: %lld\n", delta_time);
    } else {
        printf("error: %d\n", res);
    }
}

int main(void) {
    printf("Scanning signature...\n");

    test_timed();
    return 0;

    struct signature signature;
    struct timespec timeout = { .tv_sec = 10, .tv_nsec = 0 };

    int res = scan_signature(&signature, &timeout);
    if (res == PE_OK) {
        printf("signature: <%04lx.%04lx>\n", *(size_t*)signature.image_data, *(size_t*)(signature.image_data + sizeof(size_t)));
    } else if (res == PE_NODEV) {
        printf("kernel module not loaded\n");
    } else if (res == PE_AGAIN) {
        printf("timedout\n");
    }

    return 0;
}
