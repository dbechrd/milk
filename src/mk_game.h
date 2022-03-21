#pragma once
#include "mk_common.h"
#include "mk_universe.h"

struct MK_Game {
    MK_Universe universe;
    int id_player;
};

int MK_Game_Init         (MK_Game *game);
int MK_Game_PlayerInit   (MK_Game *game);
int MK_Game_PlayerGetPos (MK_Game *game, MK_Attr_Position *pos);
int MK_Game_PlayerMove   (MK_Game *game, int x, int y);