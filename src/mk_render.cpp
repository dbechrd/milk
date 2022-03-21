#include "mk_context.h"
#include "mk_render.h"

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 0, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));

    MK_Attr_Position pos = { 0 };
    succeed_or_return_expr(MK_Game_PlayerGetPos(&ctx->game, &pos));
    SDL_Rect rect{ pos.x, pos.y, 20, 20 };
    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 255, 0, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderFillRect(ctx->rndr, &rect));

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}