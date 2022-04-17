#include <mm/mm.h>


//stolen from mint's memewm

static size_t bump_allocator_base = 0x1000000;


static void *balloc_aligned(size_t count, size_t alignment) {
    size_t new_base = bump_allocator_base;
    if (new_base & (alignment - 1)) {
        new_base &= ~(alignment - 1);
        new_base += alignment;
    }
    void *ret = (void *)new_base;
    new_base += count;
    bump_allocator_base = new_base;
    return ret;
}

void *malloc(size_t count) {
    void* addy = balloc_aligned(count, 4);
    DbgPrintFmt("malloc: %p\n", addy);
    return addy;
}

void free(void *ptr) {
    (void)ptr;
}