#ifndef _PRISM_MEMORY_UTIL_
#define _PRISM_MEMORY_UTIL_

#include <stddef.h>
#include <stdint.h>

void* check_malloc(size_t size);
void* check_calloc(size_t num, size_t size);

typedef struct MemoryArenaBlock
{
    char*                    data;
    int                      amount_used;
    struct MemoryArenaBlock* next;
} MemoryArenaBlock;

typedef struct MemoryArena
{
    int               block_size;
    MemoryArenaBlock* curr_block;
} MemoryArena;

MemoryArena memory_arena_create(int block_size);
void        memory_arena_destroy(MemoryArena arena);

void* memory_arena_allocate(MemoryArena* arena, int size);

#endif // _PRISM_MEMORY_UTIL_