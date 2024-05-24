
#include <assert.h>
#include <unistd.h>
#include "malloc_2.h"

int main() {
    void *base = sbrk(0);
    char *a = (char *)scalloc(1e8, 1);
    assert(a != nullptr);
    void *after = sbrk(0);
    assert(1e8 + _size_meta_data() == (size_t)after - (size_t)base);

    char *b = (char *)scalloc(1e8 + 1, 1);
    assert(b == nullptr);
    after = sbrk(0);
    assert(1e8 + _size_meta_data() == (size_t)after - (size_t)base);

    sfree(a);
}
