#include <stdio.h>
#include "fprint.h"

int main(void) {
    printf("Scanning signature...\n");

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
