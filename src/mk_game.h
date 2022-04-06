#pragma once
#include "mk_common.h"
#include "mk_universe.h"

#define MK_GAME_PHYSICS_ITERATIONS 8

#define MK_PLAYERSLOT_MAX UINT8_MAX
typedef uint8_t MK_PlayerSlot;

struct MK_Game {
    MK_Universe universe;
    MK_EntityID player_ids[MK_PLAYERSLOT_MAX];
    bool debug_no_gravity;
    bool debug_no_physics_simulation;
    bool debug_no_collision_resolution;
};

int MK_Game_Init            (MK_Game *game);
int MK_Game_WallInit        (MK_Game *game, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id);
int MK_Game_PhysicsBodyInit (MK_Game *game, MK_EntityID *id, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id);
int MK_Game_PlayerMove      (MK_Game *game, MK_PlayerSlot player_slot, float x, float y);
int MK_Game_Simulate        (MK_Game *game, float dt);
