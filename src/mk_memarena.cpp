#include "mk_memarena.h"
#include "mk_common.h"
#include <string.h>

void *MK_MemArena_Alloc(MK_MemArena *arena, size_t bytes) {
    if (arena->offset + bytes <= sizeof(arena->data)) {
        void *ptr = arena->data + arena->offset;
        arena->offset += bytes;
        return ptr;
    }
    assert_return_zero(("arena out of memory", 0));
}

void MK_MemArena_Clear(MK_MemArena *arena) {
    assert_return(arena);
    memset(arena->data, 0, sizeof(arena->data));
    arena->offset = 0;
}