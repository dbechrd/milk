#include "mk_universe.h"

#define universe_guard() \
    assert_return_guard(universe); \
    assert_return_guard(id >= 0); \
    assert_return_guard(id < MK_ENTITIES_MAX); \
    assert_return_guard(universe->exists[id]);

int MK_Universe_Create(MK_Universe *universe, int *id) {
    assert_return_guard(id);

    for (int i = 0; i < MK_ENTITIES_MAX; i++) {
        if (!universe->exists[i]) {
            universe->exists[i] = true;
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"universe full", "Failed to allocate entity", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

int MK_Universe_GetPos(MK_Universe *universe, int id, MK_Attr_Position *position) {
    universe_guard();
    assert_return_guard(position);

    MK_Attr_Position *pos = &universe->position[id];
    *position = *pos;
    return MK_SUCCESS;
}

int MK_Universe_SetPos(MK_Universe *universe, int id, const MK_Attr_Position *position) {
    universe_guard();
    assert_return_guard(position);

    MK_Attr_Position *pos = &universe->position[id];
    *pos = *position;
    return MK_SUCCESS;
}

int MK_Universe_GetColor(MK_Universe *universe, int id, MK_Attr_Color *color) {
    universe_guard();
    assert_return_guard(color);

    MK_Attr_Color *col = &universe->color[id];
    *color = *col;
    return MK_SUCCESS;
}

int MK_Universe_SetColor(MK_Universe *universe, int id, const MK_Attr_Color *color) {
    universe_guard();
    assert_return_guard(color);

    MK_Attr_Color *col = &universe->color[id];
    *col = *color;
    return MK_SUCCESS;
}

#undef universe_guard