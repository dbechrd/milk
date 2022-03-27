#include "mk_universe.h"

int MK_Color_Create(MK_Universe *universe, uint8_t *id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    assert_return_guard(id);

    for (uint8_t i = 0; i < mk_array_count(universe->c_color); i++) {
        if (!universe->c_color_exists[i]) {
            universe->c_color_exists[i] = true;
            universe->c_color[i] = { r, g, b, a };
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"color catalog full", "Failed to allocate color", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

int MK_Color_Find(MK_Universe *universe, uint8_t id, MK_Color **color) {
    assert_return_guard(universe);
    assert_return_guard(id >= 0);
    assert_return_guard(id < mk_array_count(universe->c_color));

    if (universe->c_color_exists[id]) {
        *color = &universe->c_color[id];
    } else {
        assert_return_guard(universe->c_color_exists[0]);
        *color = &universe->c_color[0];
    }
    return MK_SUCCESS;
}

//==================================================================================

int MK_Substance_Create(MK_Universe *universe, uint8_t *id, float density) {
    assert_return_guard(id);

    for (uint8_t i = 0; i < mk_array_count(universe->c_substance); i++) {
        if (!universe->c_substance_exists[i]) {
            universe->c_substance_exists[i] = true;
            universe->c_substance[i].density = density;
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"substance catalog full", "Failed to allocate substance", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

int MK_Substance_Find(MK_Universe *universe, uint8_t id, MK_Substance **substance) {
    assert_return_guard(universe);
    assert_return_guard(id >= 0);
    assert_return_guard(id < mk_array_count(universe->c_substance));

    if (universe->c_substance_exists[id]) {
        *substance = &universe->c_substance[id];
    } else {
        assert_return_guard(universe->c_substance_exists[0]);
        *substance = &universe->c_substance[0];
    }
    return MK_SUCCESS;
}

//==================================================================================

int MK_Universe_Init(MK_Universe *universe) {
    uint8_t default_color = 0;
    succeed_or_return_expr(MK_Color_Create(universe, &default_color, 255, 255, 255, 255));
    assert_return_guard(default_color == 0);

    uint8_t default_substance = 0;
    succeed_or_return_expr(MK_Substance_Create(universe, &default_substance, 0.0f));
    assert_return_guard(default_substance == 0);

    return MK_SUCCESS;
}

int MK_Universe_Create(MK_Universe *universe, int *id) {
    assert_return_guard(id);

    for (int i = 0; i < mk_array_count(universe->e_exists); i++) {
        if (!universe->e_exists[i]) {
            universe->e_exists[i] = true;
            *id = i;
            return MK_SUCCESS;
        }
    }

    assert_return_err(!"universe full", "Failed to allocate entity", MK_ERR_BAD_ALLOC);
    return MK_SUCCESS;
}

#define universe_guard() \
    assert_return_guard(id >= 0); \
    assert_return_guard(id < mk_array_count(universe->e_exists)); \
    assert_return_guard(universe->e_exists[id]);

int MK_Universe_ColorId(MK_Universe *universe, int id, uint8_t **color_id)
{
    universe_guard();
    *color_id = &universe->e_color[id];
    return MK_SUCCESS;
}

int MK_Universe_SubstanceId(MK_Universe *universe, int id, uint8_t **substance_id)
{
    universe_guard();
    *substance_id = &universe->e_substance[id];
    return MK_SUCCESS;
}

int MK_Universe_Color(MK_Universe *universe, int id, MK_Color **color) {
    universe_guard();
    succeed_or_return_expr(MK_Color_Find(universe, universe->e_color[id], color));
    return MK_SUCCESS;
}

int MK_Universe_Substance(MK_Universe *universe, int id, MK_Substance **substance) {
    universe_guard();
    succeed_or_return_expr(MK_Substance_Find(universe, universe->e_substance[id], substance));
    return MK_SUCCESS;
}

int MK_Universe_Position(MK_Universe *universe, int id, MK_Vec2 **position) {
    universe_guard();
    *position = &universe->e_position[id];
    return MK_SUCCESS;
}

int MK_Universe_Size(MK_Universe *universe, int id, MK_Vec2 **size) {
    universe_guard();
    *size = &universe->e_size[id];
    return MK_SUCCESS;
}

int MK_Universe_Mass(MK_Universe *universe, int id, MK_Mass **mass) {
    universe_guard();
    *mass = &universe->e_mass[id];
    return MK_SUCCESS;
}

int MK_Universe_Health(MK_Universe *universe, int id, MK_Health **health) {
    universe_guard();
    *health = &universe->e_health[id];
    return MK_SUCCESS;
}

#undef universe_guard