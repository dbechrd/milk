#include "mk_universe.h"

int MK_Universe_Init(MK_Universe *universe) {
    uint8_t default_color = MK_Color_Create(universe, 255, 255, 255, 255);
    uint8_t default_substance = MK_Substance_Create(universe, 0.0001f);

    assert_return_guard(default_color == 0);
    assert_return_guard(default_substance == 0);
    return MK_SUCCESS;
}

int MK_Universe_Create(MK_Universe *universe, MK_EntityID *id, uint16_t flags) {
    for (MK_EntityID i = 0; i < mk_array_count(universe->e_exists); i++) {
        if (!universe->e_exists[i]) {
            universe->e_exists[i] = flags;
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"universe full", "Failed to allocate entity", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

MK_ColorID MK_Color_Create(MK_Universe *universe, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    assert_abort(a);  // alpha == 0 is how we denote an empty slot

    for (uint8_t i = 0; i < mk_array_count(universe->c_color); i++) {
        if (!universe->c_color[i].a) {
            universe->c_color[i] = { r, g, b, a };
            return i;
        }
    }

    mk_log_info("Color catalog full. Failed to add color [%u, %u, %u, %u]. Returning default color instead.\n", r, g, b, a);
    return 0;
}

MK_SubstanceID MK_Substance_Create(MK_Universe *universe, float density)
{
    assert_abort(density);  // density == 0 is how we denote an empty slot

    for (uint8_t i = 0; i < mk_array_count(universe->c_substance); i++) {
        if (!universe->c_substance[i].density) {
            universe->c_substance[i].density = density;
            return i;
        }
    }

    mk_log_info("Substance catalog full. Failed to add substance [density: %f]. Returning default substance instead.\n", density);
    return 0;
}