#include "mk_game.h"

#define game_guard() \
    assert_return_guard(player_slot >= 0); \
    assert_return_guard(player_slot < MK_GAME_PLAYERS_MAX);

int MK_Game_Init(MK_Game *game) {
#if _DEBUG
    //game->debug_no_collision_resolution = true;
#endif

    succeed_or_return_expr(MK_Universe_Init(&game->universe));

    uint8_t substance_wood_id = MK_Substance_Create(&game->universe, 0.0001f);

    uint8_t color_red_id   = MK_Color_Create(&game->universe, 255, 110, 110, 255);
    uint8_t color_green_id = MK_Color_Create(&game->universe, 110, 255, 110, 255);
    uint8_t color_blue_id  = MK_Color_Create(&game->universe, 110, 110, 255, 255);

    succeed_or_return_expr(MK_Game_PlayerInit(game, 0, { 100.0f, 100.0f }, 100.0f, 100.0f, color_red_id, substance_wood_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 1, { 500.0f, 100.0f }, 200.0f, 150.0f, color_green_id, substance_wood_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 2, { 300.0f, 200.0f }, 150.0f,  50.0f, color_blue_id, substance_wood_id));
    return MK_SUCCESS;
}

int MK_Game_PlayerInit(MK_Game *game, int player_slot, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    game_guard();

    uint16_t flags = MK_E_COLOR | MK_E_SUBSTANCE | MK_E_POSITION | MK_E_SIZE | MK_E_MASS | MK_E_HEALTH;
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &game->player_ids[player_slot], flags));

    uint8_t   *e_col_id   = MK_Universe_ColorId(&game->universe, game->player_ids[player_slot]);
    uint8_t   *e_subst_id = MK_Universe_SubstanceId(&game->universe, game->player_ids[player_slot]);
    MK_Vec2   *e_pos      = MK_Universe_Position(&game->universe, game->player_ids[player_slot]);
    MK_Vec2   *e_size     = MK_Universe_Size(&game->universe, game->player_ids[player_slot]);
    MK_Mass   *e_mass     = MK_Universe_Mass(&game->universe, game->player_ids[player_slot]);
    MK_Health *e_health   = MK_Universe_Health(&game->universe, game->player_ids[player_slot]);
    assert_return_guard(e_col_id);
    assert_return_guard(e_subst_id);
    assert_return_guard(e_pos);
    assert_return_guard(e_size);
    assert_return_guard(e_mass);
    assert_return_guard(e_health);

    *e_col_id = color_id;
    *e_subst_id = substance_id;

    MK_Substance *substance = MK_Substance_Find(&game->universe, *e_subst_id);
    assert_return_guard(substance);

    *e_pos = position;
    e_size->x = w;
    e_size->y = h;
    float mass = substance->density * e_size->x * e_size->y;
    e_mass->invMass = mass ? 1.0f / mass : 0;
    e_health->health = 100;
    e_health->maxHealth = 100;
    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, int player_slot, float x, float y) {
    game_guard();

    MK_Vec2 *e_pos = MK_Universe_Position(&game->universe, game->player_ids[player_slot]);
    assert_return_guard(e_pos);
    e_pos->x += x;
    e_pos->y += y;
    return MK_SUCCESS;
}

struct MK_Manifold {
    float x;
    float y;
};

bool MK_Intersects(MK_Manifold *manifold, MK_Vec2 *pos1, MK_Vec2 *siz1,
                   MK_Vec2 *pos2, MK_Vec2 *siz2) {
    assert_return_false(manifold);
    assert_return_false(pos1);
    assert_return_false(siz1);
    assert_return_false(pos2);
    assert_return_false(siz2);

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

    MK_Vec2 *e_pos1 = 0;
    MK_Vec2 *e_siz1 = 0;
    MK_Mass *e_mas1 = 0;
    MK_Vec2 *e_pos2 = 0;
    MK_Vec2 *e_siz2 = 0;
    MK_Mass *e_mas2 = 0;

    for (int iter = 0; iter < MK_GAME_PHYSICS_ITERATIONS; iter++) {
        for (int i = 0 ; i < MK_GAME_PLAYERS_MAX; i++) {
            e_pos1 = MK_Universe_Position(&game->universe, game->player_ids[i]);
            e_siz1 = MK_Universe_Size(&game->universe, game->player_ids[i]);
            e_mas1 = MK_Universe_Mass(&game->universe, game->player_ids[i]);
            if (!(e_pos1 && e_siz1 && e_mas1)) {
                continue;
            }
            for (int j = i + 1; j < MK_GAME_PLAYERS_MAX; j++) {
                e_mas2 = MK_Universe_Mass(&game->universe, game->player_ids[j]);
                e_pos2 = MK_Universe_Position(&game->universe, game->player_ids[j]);
                e_siz2 = MK_Universe_Size(&game->universe, game->player_ids[j]);
                if (!(e_pos2 && e_siz2 && e_mas2)) {
                    continue;
                }

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