#include "mk_context.h"
#include "mk_render.h"

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 0, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));

    for (int i = 0; i < MK_GAME_PLAYERS_MAX; i++) {
        if (ctx->game.universe.exists[i]) {
            SDL_Point pos{};
            succeed_or_return_expr(MK_Game_PlayerGetPos(&ctx->game, i, &pos));
            SDL_Color col{};
            succeed_or_return_expr(MK_Game_PlayerGetColor(&ctx->game, i, &col));
            SDL_Rect rect{ pos.x, pos.y, 20, 20 };
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, col.r, col.g, col.b, col.a));
            succeed_or_return_expr_sdl(SDL_RenderFillRect(ctx->rndr, &rect));
        }
    }

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}