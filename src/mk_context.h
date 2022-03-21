#pragma once
#include "mk_common.h"
#include "mk_game.h"
#include "mk_memarena.h"
#include "mk_universe.h"

struct SDL_Window;
struct SDL_Renderer;

struct MK_Context {
    SDL_Window *win;
    SDL_Renderer *rndr;
    MK_MemArena *temp;           // pointer to currently active temp storage
    MK_MemArena tempBuffers[2];  // zeroed every other frame (double buffered)

    bool quit;
    MK_Game game;
};