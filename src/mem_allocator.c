#include <stdio.h>
#include <unistd.h>
#include "memory_tools.h"

#define MMAP_THRESHOLD 128 * 1024
#define BLOCK_SIZE (sizeof(struct mem_block))
#define INCREMENT 256

const size_t BLOCK_INFO = sizeof(struct mem_block);
static struct mem_block *free_list_head = NULL;
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
        if (free_list_head == NULL){
            remainder->next = NULL;
            free_list_head = remainder;
        }
        else{
            free_list_head->prev = remainder;
            remainder->next = free_list_head;
            free_list_head = remainder;
        }

        return (void*)(block + 1);
    }
    block->isFree = 0;
    return (void*)(block +1);
}


void ecoalesce(struct mem_block *block){
    void *heap_border = sbrk(0);
    struct mem_block *right_block = (struct mem_block*)((char*)block + BLOCK_SIZE + block->size); 

    if ((void*) right_block < heap_border && right_block->isFree == 1){
        block->size += BLOCK_SIZE + right_block->size;
    
        if(right_block->prev != NULL){
            right_block->prev->next = right_block->next;
        }
        else{
            free_list = right_block->next;
        }
        if(right_block->next != NULL){
            right_block->next->prev = right_block->prev;
        }
    }

    struct mem_block *curr = free_list;

    while(curr != NULL){
        struct mem_block *right_curr = (struct mem_block*)((char*)curr + BLOCK_SIZE + block->size);

        if(right_curr == block){
            curr->size += BLOCK_SIZE + right_curr->size;

            if (block->prev != NULL){
                block->prev->next = block->next;
            }
            else{
                free_list = block;
            }

            if(block->next != NULL){
                block->next->prev = block->prev;
            }
            break;
        }
        curr = curr->next;
    }
    
}

void *emalloc(size_t size){
    if (size <= 0){
        printf("invalid size");
        return NULL;
    }

    size = (size + 7) & ~7; // align the bytes
    size_t TOTAL_SIZE = size + BLOCK_SIZE; 
    
    struct mem_block *curr_block = free_list_head;

    // employs a first fit algorithm
    while(curr_block != NULL){
       if(TOTAL_SIZE <= curr_block->size){

            if (curr_block->prev != NULL){
                curr_block->prev->next = curr_block->next;
            }
            else{
                free_list_head = curr_block->next;
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

    return esplit(new_block, size); // no need to add new_block to list, esplit handles that.
}

void efree(void *ptr){
    if (ptr == NULL){
        return;
    }
 
    struct mem_block *put_back = (struct mem_block*)((char *)ptr - BLOCK_SIZE);
    put_back->isFree = 1; 

    if (free_list_head == NULL){
        free_list_head = put_back;
        put_back->next = NULL;
        put_back->prev = NULL;
    }
    else{
        free_list_head->prev = put_back;
        put_back->next = free_list_head; 
        put_back->prev = NULL;
        free_list_head = put_back;
    }


    //coaslescing



}
