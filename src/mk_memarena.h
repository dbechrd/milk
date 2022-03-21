#pragma once
#include "mk_common.h"

struct MK_MemArena {
    size_t offset;           // bytes used
    char data[1024 * 1024];  // 1 MB hard-coded buffer (may want to make this dynamically grow)
};

void * MK_MemArena_Alloc (MK_MemArena *arena, size_t bytes);
void   MK_MemArena_Clear (MK_MemArena *arena);