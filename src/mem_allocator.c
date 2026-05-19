#include <stdio.h>
#include <unistd.h>
#include "memory_tools.h"

#define MMAP_THRESHOLD 128 * 1024
#define BLOCK_SIZE (sizeof(struct mem_block))

const size_t BLOCK_INFO = sizeof(struct mem_block);
static struct mem_block *free_list = NULL;

void printf_debug(size_t bytes){
    printf("Allocated %zu bytes\n", bytes);
}


void *emalloc(size_t size){
    if (size <= 0){
        printf("invalid size");
        return NULL;
    }

    size = (size + 7) & ~7;
    size_t TOTAL_SIZE = size + BLOCK_SIZE; // needed or not?
    
    struct mem_block *curr_block = free_list;
    struct mem_block *prev_block = NULL;

    while(curr_block){
       if(TOTAL_SIZE <= curr_block->size){

       }
        
    }

    // couldn't find avail memory size, expand program break
    if (curr_block == NULL){
        struct mem_block *new_block = (struct mem_block *)sbrk(256); 
        if(new_block == (void*)-1){
            return NULL;
        }
    }

    struct mem_block *block = (struct mem_block*)new_block;
    block->size = size;

    return (void*)(block + 1);
}

void efree(void *ptr){
    if (ptr == NULL){
        return;
    }

    struct mem_block *block = (struct mem_block*)ptr - 1;

    // Add block to the beginning of the list.
    block->next = free_list;
    free_list = block;
}
