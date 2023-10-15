#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arenaria.h"

static ArenariaBlock *arenaria_arena_get_last_block(ArenariaArena *arena);
static int arenaria_block_can_fit(ArenariaBlock block, size_t size);
static ArenariaBlock *arenaria_arena_append_empty_block(ArenariaArena *arena, int capacity);
static int arenaria_max_int(int a, int b);

ArenariaArena arenaria_arena_create(void *((*allocator)(size_t)), void (*freeer)(void *)) {
    ArenariaArena arena;
    arena.blocks = NULL;
    arena.allocator = allocator;
    arena.freeer = freeer;
    arena.length = 0;
    arena.capacity = 0;
    return arena;
}

void arenaria_arena_destroy(ArenariaArena *arena) {
    for (int i = 0; i < arena->length; i++) {
        void *block_data = arena->blocks[i].data;
        if (block_data != NULL) {
            arena->freeer(block_data);
        }
    }
    arena->freeer(arena->blocks);
    arena->blocks = NULL;
    arena->allocator = NULL;
    arena->freeer = NULL;
    arena->length = 0;
    arena->capacity = 0;
}

void *arenaria_malloc(ArenariaArena *arena, size_t size) {
    ArenariaBlock *last_block = arenaria_arena_get_last_block(arena);
    if (arena->length == 0 || !arenaria_block_can_fit(*last_block, size)) {
        last_block = arenaria_arena_append_empty_block(arena, arenaria_max_int(size, ARENARIA_MIN_BLOCK_SIZE));
        if (last_block == NULL) {
            return NULL;
        }
    }
    if (last_block->data == NULL) {
        return NULL;
    }
    void *result = last_block->data + last_block->length;
    last_block->length += size;
    return result;
}

static ArenariaBlock *arenaria_arena_get_last_block(ArenariaArena *arena) {
    return (arena->blocks + arena->length - 1);
}

static ArenariaBlock *arenaria_arena_append_empty_block(ArenariaArena *arena, int capacity) {
    if (arena->length >= arena->capacity) {
        int old_capacity = arena->capacity;
        int new_capacity = old_capacity * 2;
        if (new_capacity == 0) {
            new_capacity = ARENARIA_INITIAL_ARENA_CAPACITY;
        }
        ArenariaBlock *old_blocks = arena->blocks;
        ArenariaBlock *new_blocks = (ArenariaBlock *) arena->allocator(sizeof(ArenariaBlock) * new_capacity);
        if (new_blocks == NULL) {
            return NULL;
        }
        if (old_blocks != NULL) {
            memcpy(new_blocks, old_blocks, old_capacity);
            arena->freeer(old_blocks);
        }
        arena->capacity = new_capacity;
        arena->blocks = new_blocks;
    }
    arena->length++;
    ArenariaBlock *last = arenaria_arena_get_last_block(arena);
    last->length = 0;
    last->capacity = 0;
    last->data = NULL;
    void *new_block_data = arena->allocator(capacity);
    if (new_block_data != NULL) {
        last->capacity = capacity;
        last->data = new_block_data;
    }
    return last;
}

static int arenaria_block_can_fit(ArenariaBlock block, size_t size) {
    return block.length + size < block.capacity;
}

static int arenaria_max_int(int a, int b) {
    return a > b ? a : b;
}