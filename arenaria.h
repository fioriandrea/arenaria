#ifndef ARENARIA_H
#define ARENARIA_H

typedef struct {
    void *data;
    int length;
    int capacity;
} ArenariaBlock;

typedef struct {
    ArenariaBlock *blocks;
    int length;
    int capacity;
    void *((*allocator)(size_t));
    void (*freeer)(void *);
} ArenariaArena;

#define MIN_BLOCK_SIZE 2048
#define INITIAL_ARENA_CAPACITY 16

ArenariaArena arenaria_arena_create(void *((*allocator)(size_t)), void (*freeer)());
void *arenaria_malloc(ArenariaArena *arena, size_t size);
void arenaria_arena_destroy(ArenariaArena arena);

#endif