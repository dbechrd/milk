#pragma once
#include "mk_common.h"
#include "mk_universe.h"

#define MK_GAME_PHYSICS_ITERATIONS 5

#define MK_PLAYERSLOT_MAX UINT8_MAX
typedef uint8_t MK_PlayerSlot;

struct MK_Game {
    MK_Universe universe;
    MK_EntityID player_ids[MK_PLAYERSLOT_MAX];
    bool debug_no_collision_resolution;
};

int MK_Game_Init                 (MK_Game *game);
int MK_Game_PlayerInit           (MK_Game *game, MK_PlayerSlot player_slot, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id);
int MK_Game_PlayerMove           (MK_Game *game, MK_PlayerSlot player_slot, float x, float y);
int MK_Game_PositionalCorrection (MK_Game *game);