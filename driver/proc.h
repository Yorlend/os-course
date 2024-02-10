#pragma once

#include <linux/types.h>

int register_fprint_proc_file(void);
void unregister_fprint_proc_file(void);

// wakes up blocked reader to provide him with new data
void post_signature_data(const void* data, size_t size);
