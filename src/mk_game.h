#pragma once
#include "mk_common.h"
#include "mk_universe.h"

#define MK_GAME_PLAYERS_MAX 2

struct MK_Game {
    MK_Universe universe;
    int player_ids[MK_GAME_PLAYERS_MAX];
};

int MK_Game_Init           (MK_Game *game);
int MK_Game_PlayerInit     (MK_Game *game, int player_slot);
int MK_Game_PlayerGetPos   (MK_Game *game, int player_slot, SDL_Point *position);
int MK_Game_PlayerSetPos   (MK_Game *game, int player_slot, const SDL_Point position);
int MK_Game_PlayerGetColor (MK_Game *game, int player_slot, SDL_Color *col);
int MK_Game_PlayerSetColor (MK_Game *game, int player_slot, const SDL_Color col);
int MK_Game_PlayerMove     (MK_Game *game, int player_slot, int x, int y);