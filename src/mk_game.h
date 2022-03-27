#pragma once
#include "mk_common.h"
#include "mk_universe.h"

#define MK_GAME_PLAYERS_MAX        3
#define MK_GAME_PHYSICS_ITERATIONS 5

struct MK_Game {
    MK_Universe universe;
    int player_ids[MK_GAME_PLAYERS_MAX];
    bool debug_no_collision_resolution;
};

int MK_Game_Init                 (MK_Game *game);
int MK_Game_PlayerInit           (MK_Game *game, int player_slot, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id);
int MK_Game_PlayerMove           (MK_Game *game, int player_slot, float x, float y);
int MK_Game_PositionalCorrection (MK_Game *game);