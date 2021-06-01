#include "common/platform.h"
#include <stdlib.h>
#include <sys/malloc.h>

void *sr_malloc(const size_t size) {
    return malloc(size);
}

void *sr_realloc(void *const ptr, const size_t size) {
    return realloc(ptr, size);
}

int sr_free(void *const ptr) {
    free(ptr);
    return 0;
}
