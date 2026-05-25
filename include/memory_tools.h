void *emalloc(size_t);
void efree(void *);

void printf_debug(size_t);

struct mem_block {
    size_t size;
    struct mem_block* next;
    struct mem_block *prev;
    int isFree = 1; // 1 = true, 0 = false
};
