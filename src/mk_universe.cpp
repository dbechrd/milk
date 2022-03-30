#include "mk_universe.h"

uint8_t MK_Color_Create(MK_Universe *universe, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    assert_return_zero(universe);

    for (uint8_t i = 0; i < mk_array_count(universe->c_color); i++) {
        if (!universe->c_color_exists[i]) {
            universe->c_color_exists[i] = true;
            universe->c_color[i] = { r, g, b, a };
            return i;
        }
    }

    mk_log_info("Color catalog full. Failed to add color [%u, %u, %u, %u]. Returning default color instead.\n", r, g, b, a);
    return 0;
}

MK_Color *MK_Color_Find(MK_Universe *universe, uint8_t id) {
    assert_return_zero(universe);
    assert_return_zero(id >= 0);
    assert_return_zero(id < mk_array_count(universe->c_color));

    if (universe->c_color_exists[id]) {
        return &universe->c_color[id];
    } else {
        // return default color
        assert_return_zero(universe->c_color_exists[0]);
        return &universe->c_color[0];
    }
}

//==================================================================================

uint8_t MK_Substance_Create(MK_Universe *universe, float density) {
    assert_return_zero(universe);

    for (uint8_t i = 0; i < mk_array_count(universe->c_substance); i++) {
        if (!universe->c_substance_exists[i]) {
            universe->c_substance_exists[i] |= true;
            universe->c_substance[i].density = density;
            return i;
        }
    }

    mk_log_info("Substance catalog full. Failed to add substance [density: %f]. Returning default substance instead.\n", density);
    return 0;
}

MK_Substance *MK_Substance_Find(MK_Universe *universe, uint8_t id) {
    assert_return_zero(universe);
    assert_return_zero(id >= 0);
    assert_return_zero(id < mk_array_count(universe->c_substance));

    if (universe->c_substance_exists[id]) {
        return &universe->c_substance[id];
    } else {
        // return default substance
        assert_return_zero(universe->c_substance_exists[0]);
        return &universe->c_substance[0];
    }
}

//==================================================================================

int MK_Universe_Init(MK_Universe *universe) {
    uint8_t default_color = MK_Color_Create(universe, 255, 255, 255, 255);
    uint8_t default_substance = MK_Substance_Create(universe, 0.0f);

    assert_return_guard(default_color == 0);
    assert_return_guard(default_substance == 0);
    return MK_SUCCESS;
}

int MK_Universe_Create(MK_Universe *universe, int *id, uint16_t flags) {
    assert_return_guard(id);

    for (int i = 0; i < mk_array_count(universe->e_exists); i++) {
        if (!universe->e_exists[i]) {
            universe->e_exists[i] = flags;
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"universe full", "Failed to allocate entity", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

#define universe_guard_cond(flags) \
    (universe && id >= 0 && id < mk_array_count(universe->e_exists) && ((universe->e_exists[id] & (flags)) == (flags)))

uint8_t *MK_Universe_ColorId(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_COLOR));
    return &universe->e_color[id];
}

uint8_t *MK_Universe_SubstanceId(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_SUBSTANCE));
    uint8_t *substanceId = &universe->e_substance[id];
    return substanceId;
}

MK_Color *MK_Universe_Color(MK_Universe *universe, int id) {
    uint8_t *colorId = MK_Universe_ColorId(universe, id);
    MK_Color *color = MK_Color_Find(universe, colorId ? *colorId : 0u);
    return color;
}

MK_Substance *MK_Universe_Substance(MK_Universe *universe, int id) {
    uint8_t *substanceId = MK_Universe_SubstanceId(universe, id);
    MK_Substance *substance = MK_Substance_Find(universe, substanceId ? *substanceId : 0u);
    return substance;
}

MK_Vec2 *MK_Universe_Position(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_POSITION));
    MK_Vec2 *pos = &universe->e_position[id];
    return pos;
}

MK_Vec2 *MK_Universe_Size(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_SIZE));
    MK_Vec2 *size = &universe->e_size[id];
    return size;
}

MK_Mass *MK_Universe_Mass(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_MASS));
    MK_Mass *mass = &universe->e_mass[id];
    return mass;
}

MK_Health *MK_Universe_Health(MK_Universe *universe, int id) {
    assert_return_zero(universe_guard_cond(MK_E_HEALTH));
    MK_Health *health = &universe->e_health[id];
    return health;
}

#undef universe_guard