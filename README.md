# arenaria

Memory Arena Allocator in C.

# Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arenaria.h"

int main() {
    ArenariaArena arena = arenaria_arena_create(malloc, free);

    arenaria_malloc(&arena, 1000);
    arenaria_malloc(&arena, 2000);
    arenaria_malloc(&arena, 3000);
    arenaria_malloc(&arena, 4000);

    arenaria_arena_destroy(&arena);

    return 0;
}
```
