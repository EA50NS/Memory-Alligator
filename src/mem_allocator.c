#include <stdio.h>
#include <unistd.h>
#include "memory_tools.h"

#define MMAP_THRESHOLD 128 * 1024
#define BLOCK_SIZE (sizeof(struct mem_block))
#define INCREMENT 256
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

    size = (size + 7) & ~7; // align the bytes
    size_t TOTAL_SIZE = size + BLOCK_SIZE; 
    
    struct mem_block *curr_block = free_list;

    // employs a first fit algorithm
    while(curr_block != NULL){
       if(TOTAL_SIZE <= curr_block->size){

            if (curr_block->prev != NULL){
                curr_block->prev->next = curr_block->next;
            }
            else{
                free_list = curr_block->next;
            }
            return (void *)(curr_block + 1);
       }
    }
    
    struct mem_block *new_block = (struct mem_block *)sbrk(INCREMENT);
    if (new_block == (void*) - 1){
        printf("sbrk failed \n");
        return NULL;
    }
    new_block->size = INCREMENT-BLOCK_SIZE;
    new_block->next = NULL;
    new_block->prev = NULL:
    new_block->isFree = 0;

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
