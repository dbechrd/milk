#include "mk_game.h"

#define game_guard() \
    assert_return_guard(player_slot >= 0); \
    assert_return_guard(player_slot < MK_GAME_PLAYERS_MAX);

int MK_Game_Init(MK_Game *game) {
#if _DEBUG
    //game->debug_no_collision_resolution = true;
#endif

    succeed_or_return_expr(MK_Universe_Init(&game->universe));

    uint8_t substance_wood_id = 0;
    succeed_or_return_expr(MK_Substance_Create(&game->universe, &substance_wood_id, 0.0001f));

    uint8_t color_red_id = 0;
    uint8_t color_green_id = 0;
    uint8_t color_blue_id = 0;
    succeed_or_return_expr(MK_Color_Create(&game->universe, &color_red_id, 255, 110, 110, 255));
    succeed_or_return_expr(MK_Color_Create(&game->universe, &color_green_id, 110, 255, 110, 255));
    succeed_or_return_expr(MK_Color_Create(&game->universe, &color_blue_id, 110, 110, 255, 255));

    succeed_or_return_expr(MK_Game_PlayerInit(game, 0, { 100.0f, 100.0f }, 100.0f, 100.0f, color_red_id, substance_wood_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 1, { 500.0f, 100.0f }, 200.0f, 150.0f, color_green_id, substance_wood_id));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 2, { 300.0f, 200.0f }, 150.0f,  50.0f, color_blue_id, substance_wood_id));
    return MK_SUCCESS;
}

int MK_Game_PlayerInit(MK_Game *game, int player_slot, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    game_guard();

    succeed_or_return_expr(MK_Universe_Create(&game->universe, &game->player_ids[player_slot]));

    uint8_t *col_id = 0;
    uint8_t *subst_id = 0;
    MK_Vec2 *pos = 0;
    MK_Vec2 *siz = 0;
    MK_Mass *mas = 0;
    MK_Health *health = 0;
    succeed_or_return_expr(MK_Universe_ColorId(&game->universe, game->player_ids[player_slot], &col_id));
    succeed_or_return_expr(MK_Universe_SubstanceId(&game->universe, game->player_ids[player_slot], &subst_id));
    succeed_or_return_expr(MK_Universe_Position(&game->universe, game->player_ids[player_slot], &pos));
    succeed_or_return_expr(MK_Universe_Size(&game->universe, game->player_ids[player_slot], &siz));
    succeed_or_return_expr(MK_Universe_Mass(&game->universe, game->player_ids[player_slot], &mas));
    succeed_or_return_expr(MK_Universe_Health(&game->universe, game->player_ids[player_slot], &health));

    *col_id = color_id;
    *subst_id = substance_id;

    MK_Substance *substance = 0;
    succeed_or_return_expr(MK_Substance_Find(&game->universe, *subst_id, &substance));

    *pos = position;
    siz->x = w;
    siz->y = h;
    float mass = substance->density * siz->x * siz->y;
    mas->invMass = mass ? 1.0f / mass : 0;
    health->health = 100;
    health->maxHealth = 100;
    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, int player_slot, float x, float y) {
    game_guard();

    MK_Vec2 *pos = 0;
    succeed_or_return_expr(MK_Universe_Position(&game->universe, game->player_ids[player_slot], &pos));
    pos->x += x;
    pos->y += y;
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

    MK_Vec2 *pos1 = 0;
    MK_Vec2 *siz1 = 0;
    MK_Mass *mas1 = 0;
    MK_Vec2 *pos2 = 0;
    MK_Vec2 *siz2 = 0;
    MK_Mass *mas2 = 0;

    for (int iter = 0; iter < MK_GAME_PHYSICS_ITERATIONS; iter++) {
        for (int i = 0 ; i < MK_GAME_PLAYERS_MAX; i++) {
            succeed_or_return_expr(MK_Universe_Position(&game->universe, game->player_ids[i], &pos1));
            succeed_or_return_expr(MK_Universe_Size(&game->universe, game->player_ids[i], &siz1));
            succeed_or_return_expr(MK_Universe_Mass(&game->universe, game->player_ids[i], &mas1));
            for (int j = i + 1; j < MK_GAME_PLAYERS_MAX; j++) {
                succeed_or_return_expr(MK_Universe_Mass(&game->universe, game->player_ids[j], &mas2));
                if (!mas1->invMass && !mas2->invMass) {
                    // both are immovable
                    continue;
                }

                succeed_or_return_expr(MK_Universe_Position(&game->universe, game->player_ids[j], &pos2));
                succeed_or_return_expr(MK_Universe_Size(&game->universe, game->player_ids[j], &siz2));
                MK_Manifold manifold = { 0 };

                if (MK_Intersects(&manifold, pos1, siz1, pos2, siz2)) {
                    if (!mas1->invMass) {
                        pos2->x += manifold.x;
                        pos2->y += manifold.y;
                    } else if (!mas2->invMass) {
                        pos1->x -= manifold.x;
                        pos1->y -= manifold.y;
                    } else {
                        float alpha = mas1->invMass / (mas1->invMass + mas2->invMass);
                        pos1->x -= manifold.x * alpha;
                        pos1->y -= manifold.y * alpha;
                        pos2->x += manifold.x * (1.0f - alpha);
                        pos2->y += manifold.y * (1.0f - alpha);
                    }
                }
            }
        }
    }
    return MK_SUCCESS;
}