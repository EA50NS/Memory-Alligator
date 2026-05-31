#include <stdio.h>
#include <unistd.h>
#include "memory_tools.h"

#define MMAP_THRESHOLD 128 * 1024
#define BLOCK_SIZE (sizeof(struct mem_block))
#define INCREMENT 256

const size_t BLOCK_INFO = sizeof(struct mem_block);
static struct mem_block *free_list = NULL;
static struct mem_block *chosen_block = NULL;

void printf_debug(size_t bytes){
    printf("Allocated %zu bytes\n", bytes);
}


void *esplit(struct mem_block *block, size_t size){
    if(block->size >= size + BLOCK_SIZE + 8){
        struct mem_block *remainder = (struct mem_block*)((char *)(block+1) + size);
        remainder->size = block->size - size - BLOCK_SIZE;
        remainder->prev = NULL;
        remainder->next = NULL;
        remainder->isFree = 1;

        block->size = size;
        block->isFree = 0;

        // add remainder block to free list
        if (free_list == NULL){
            remainder->next = NULL;
            free_list = remainder;
        }
        else{
            free_list->prev = remainder;
            remainder->next = free_list;
            free_list = remainder;
        }

        return (void*)(block + 1);
    }
    block->isFree = 0;
    return (void*)(block +1);
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

            if (curr_block->next != NULL){
                curr_block->next->prev = curr_block->prev;
            }
            // should something different happen if next IS null?

            chosen_block = curr_block;
            return esplit(chosen_block, size);

       }
       curr_block = curr_block->next;
    }
    
    struct mem_block *new_block = (struct mem_block *)sbrk(INCREMENT);
    if (new_block == (void*) - 1){
        printf("sbrk failed \n");
        return NULL;
    }
    new_block->size = INCREMENT-BLOCK_SIZE;
    new_block->next = NULL;
    new_block->prev = NULL;
    new_block->isFree = 0;

    esplit(new_block, size); // no need to add new_block to list, esplit handles that.
    return (void*)(new_block + 1);
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
