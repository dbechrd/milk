#include "mk_universe.h"

int MK_Universe_Create(MK_Universe *universe, int *id) {
    assert_return_guard(id);

    for (int i = 0; i < MK_ENTITIES_MAX; i++) {
        if (!universe->exists[i]) {
            universe->exists[i] = true;
            *id = i;
            return MK_SUCCESS;
        }
    }

    return MK_ERR_BAD_ALLOC;
}

int MK_Universe_Move(MK_Universe *universe, int id, int x, int y) {
    assert_return_guard(universe);
    assert_return_guard(id >= 0);
    assert_return_guard(id < MK_ENTITIES_MAX);
    assert_return_guard(universe->exists[id]);

    MK_Attr_Position *pos = &universe->position[id];
    pos->x += x;
    pos->y += y;
    return MK_SUCCESS;
}