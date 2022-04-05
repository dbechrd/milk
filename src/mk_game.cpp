#include "mk_game.h"

int MK_Game_Init(MK_Game *game) {
#if _DEBUG
    //game->debug_no_collision_resolution = true;
#endif

    succeed_or_return_expr(MK_Universe_Init(&game->universe));

    uint8_t color_fence_id = MK_Color_Create(&game->universe,  80,  40,   0, 255);
    uint8_t color_red_id   = MK_Color_Create(&game->universe, 255, 110, 110, 255);
    uint8_t color_green_id = MK_Color_Create(&game->universe, 110, 255, 110, 255);
    uint8_t color_blue_id  = MK_Color_Create(&game->universe, 110, 110, 255, 255);

    succeed_or_return_expr(MK_Game_WallInit(game, {        10,       10}, 1600 - 20,       10, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, {        10, 900 - 20}, 1600 - 20,       10, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, {        10,       10},        10, 900 - 20, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, { 1600 - 20,       10},        10, 900 - 20, color_fence_id, 0));

    uint8_t substance_player_id = MK_Substance_Create(&game->universe, 0.0001f);
    succeed_or_return_expr(MK_Game_PlayerInit(game, 0, { 100.0f, 100.0f }, 100.0f, 100.0f, color_red_id,   substance_player_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 1, { 500.0f, 100.0f }, 200.0f, 150.0f, color_green_id, substance_player_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 2, { 300.0f, 200.0f }, 150.0f,  50.0f, color_blue_id,  substance_player_id));

    return MK_SUCCESS;
}

int MK_Game_WallInit(MK_Game *game, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    uint16_t flags = MK_E_COLOR | MK_E_SUBSTANCE | MK_E_POSITION | MK_E_SIZE | MK_E_MASS;
    MK_EntityID entity_id = 0;
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &entity_id, flags));

    MK_Substance *substance = &game->universe.c_substance[substance_id];

    MK_Vec2   *e_pos      = &game->universe.e_position[entity_id];
    MK_Vec2   *e_size     = &game->universe.e_size[entity_id];
    MK_Mass   *e_mass     = &game->universe.e_mass[entity_id];

    game->universe.e_color[entity_id] = color_id;
    game->universe.e_substance[entity_id] = substance_id;

    *e_pos = position;
    e_size->x = w;
    e_size->y = h;
    float mass = substance->density * e_size->x * e_size->y;
    e_mass->invMass = mass ? 1.0f / mass : 0;
    return MK_SUCCESS;
}


int MK_Game_PlayerInit(MK_Game *game, MK_PlayerSlot player_slot, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    uint16_t flags = MK_E_COLOR | MK_E_SUBSTANCE | MK_E_POSITION | MK_E_SIZE | MK_E_MASS | MK_E_HEALTH;
    MK_EntityID entity_id = 0;
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &entity_id, flags));

    MK_Substance *substance = &game->universe.c_substance[substance_id];

    MK_Vec2   *e_pos      = &game->universe.e_position[entity_id];
    MK_Vec2   *e_size     = &game->universe.e_size[entity_id];
    MK_Mass   *e_mass     = &game->universe.e_mass[entity_id];
    MK_Health *e_health   = &game->universe.e_health[entity_id];

    game->universe.e_color[entity_id] = color_id;
    game->universe.e_substance[entity_id] = substance_id;

    *e_pos = position;
    e_size->x = w;
    e_size->y = h;
    float mass = substance->density * e_size->x * e_size->y;
    e_mass->invMass = mass ? 1.0f / mass : 0;
    e_health->health = 100;
    e_health->maxHealth = 100;

    game->player_ids[player_slot] = entity_id;
    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, MK_PlayerSlot player_slot, float x, float y) {
    MK_EntityID player_id = game->player_ids[player_slot];
    MK_Vec2 *e_pos = &game->universe.e_position[player_id];
    e_pos->x += x;
    e_pos->y += y;
    return MK_SUCCESS;
}

struct MK_Manifold {
    float x;
    float y;
};

bool MK_Intersects(MK_Manifold *manifold, MK_Vec2 *pos1, MK_Vec2 *siz1, MK_Vec2 *pos2, MK_Vec2 *siz2) {
    float x_overlap = mk_min(
        (pos1->x + siz1->x) - pos2->x,
        (pos2->x + siz2->x) - pos1->x
    );

    float y_overlap = mk_min(
        (pos1->y + siz1->y) - pos2->y,
        (pos2->y + siz2->y) - pos1->y
    );

    if (x_overlap < 0 || y_overlap < 0) {
        return false;
    }

    if (x_overlap < y_overlap) {
        manifold->x = x_overlap;
        if (pos1->x > pos2->x) {
            manifold->x *= -1;
        }
    } else {
        manifold->y = y_overlap;
        if (pos1->y > pos2->y) {
            manifold->y *= -1;
        }
    }

    return true;
}

int MK_Game_PositionalCorrection(MK_Game *game) {
    assert_return_zero(!game->debug_no_collision_resolution);

    for (int iter = 0; iter < MK_GAME_PHYSICS_ITERATIONS; iter++) {
        for (MK_EntityID e_id_a = 0; e_id_a < MK_ENTITYID_MAX; e_id_a++) {
            MK_Vec2 *e_pos1 = &game->universe.e_position[e_id_a];
            MK_Vec2 *e_siz1 = &game->universe.e_size[e_id_a];
            MK_Mass *e_mas1 = &game->universe.e_mass[e_id_a];
            for (MK_EntityID e_id_b = (MK_EntityID)(e_id_a + 1); e_id_b < MK_ENTITYID_MAX; e_id_b++) {
                MK_Vec2 *e_pos2 = &game->universe.e_position[e_id_b];
                MK_Vec2 *e_siz2 = &game->universe.e_size[e_id_b];
                MK_Mass *e_mas2 = &game->universe.e_mass[e_id_b];

                if (!e_mas1->invMass && !e_mas2->invMass) {
                    // both are immovable
                    continue;
                }

                MK_Manifold manifold = { 0 };

                if (MK_Intersects(&manifold, e_pos1, e_siz1, e_pos2, e_siz2)) {
                    if (!e_mas1->invMass) {
                        e_pos2->x += manifold.x;
                        e_pos2->y += manifold.y;
                    } else if (!e_mas2->invMass) {
                        e_pos1->x -= manifold.x;
                        e_pos1->y -= manifold.y;
                    } else {
                        float alpha = e_mas1->invMass / (e_mas1->invMass + e_mas2->invMass);
                        e_pos1->x -= manifold.x * alpha;
                        e_pos1->y -= manifold.y * alpha;
                        e_pos2->x += manifold.x * (1.0f - alpha);
                        e_pos2->y += manifold.y * (1.0f - alpha);
                    }
                }
            }
        }
    }
    return MK_SUCCESS;
}