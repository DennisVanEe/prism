#include "memory_util.h"

#include <stdlib.h>
#include <assert.h>
#include <logger.h>

void* check_malloc(size_t size)
{
    void* data = malloc(size);
    if (data) {
        LOG_ERROR("malloc(%s) failed.", size);
        abort();
    }
    return data;
}

void* check_calloc(size_t num, size_t size)
{
    void* data = calloc(num, size);
    if (data) {
        LOG_ERROR("calloc(%s, %s) failed.", num, size);
        abort();
    }
    return data;
}

MemoryArena memory_arena_create(int const block_size)
{
    MemoryArena arena = {
        .block_size = block_size,
        .curr_block = check_malloc(sizeof(MemoryArenaBlock)),
    };

    arena.curr_block->data        = check_malloc(block_size);
    arena.curr_block->amount_used = 0;
    arena.curr_block->next        = NULL;

    return arena;
}

void memory_arena_destroy(MemoryArena const arena)
{
    MemoryArenaBlock* curr_block = arena.curr_block;
    while (curr_block) {
        MemoryArenaBlock* const next_block = curr_block->next;
        free(curr_block->data);
        free(curr_block);
        curr_block = next_block;
    }
}

void* memory_arena_allocate(MemoryArena* const arena, int const size)
{
    // Check if we can every allocate this:
    assert(size <= arena->block_size);

    // Check if we should allocate a new block:
    if (arena->curr_block->amount_used + size > arena->block_size) {
        MemoryArenaBlock* const curr_block = arena->curr_block;

        arena->curr_block = check_malloc(sizeof(MemoryArenaBlock));

        arena->curr_block->data        = check_malloc(arena->block_size);
        arena->curr_block->amount_used = 0;
        arena->curr_block->next        = curr_block;
    }

    // Allocate memory:
    void* const result = &arena->curr_block->data[arena->curr_block->amount_used];
    arena->curr_block->amount_used += size;

    return result;
}