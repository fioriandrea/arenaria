#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arenaria.h"

static ArenariaBlock *arenaria_arena_get_last_block(ArenariaArena *arena);
static int arenaria_block_can_fit(ArenariaBlock block, size_t size);
static void arenaria_arena_append_empty_block(ArenariaArena *arena, int capacity);
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

void arenaria_arena_destroy(ArenariaArena arena) {
    for (int i = 0; i < arena.length; i++) {
        void *block_data = arena.blocks[i].data;
        if (block_data != NULL) {
            arena.freeer(block_data);
        }
    }
    arena.freeer(arena.blocks);
}

void *arenaria_malloc(ArenariaArena *arena, size_t size) {
    if (arena->length == 0 || !arenaria_block_can_fit(*arenaria_arena_get_last_block(arena), size)) {
        arenaria_arena_append_empty_block(arena, arenaria_max_int(size, MIN_BLOCK_SIZE));
    }
    ArenariaBlock *last_block = arenaria_arena_get_last_block(arena);
    void *result = last_block->data + last_block->length;
    last_block->length += size;
    return result;
}

static ArenariaBlock *arenaria_arena_get_last_block(ArenariaArena *arena) {
    return (arena->blocks + arena->length - 1);
}

static void arenaria_arena_append_empty_block(ArenariaArena *arena, int capacity) {
    if (arena->length >= arena->capacity) {
        int old_capacity = arena->capacity;
        arena->capacity *= 2;
        if (arena->capacity == 0) {
            arena->capacity = INITIAL_ARENA_CAPACITY;
        }
        ArenariaBlock *old_blocks = arena->blocks;
        arena->blocks = (ArenariaBlock *) arena->allocator(sizeof(ArenariaBlock) * arena->capacity);
        if (old_blocks != NULL) {
            memcpy(arena->blocks, old_blocks, old_capacity);
        }
    }
    arena->length++;
    ArenariaBlock *last = arenaria_arena_get_last_block(arena);
    last->length = 0;
    last->capacity = capacity;
    last->data = arena->allocator(capacity);
}

static int arenaria_block_can_fit(ArenariaBlock block, size_t size) {
    return block.length + size < block.capacity;
}

static int arenaria_max_int(int a, int b) {
    return a > b ? a : b;
}