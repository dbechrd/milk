#include "mk_context.h"
#include "mk_render.h"

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 0, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));

    for (int i = 0; i < MK_GAME_PLAYERS_MAX; i++) {
        if (ctx->game.universe.e_exists[i]) {
            MK_Vec2 *pos = MK_Universe_Position(&ctx->game.universe, ctx->game.player_ids[i]);
            MK_Vec2 *size = MK_Universe_Size(&ctx->game.universe, ctx->game.player_ids[i]);
            MK_Color *col = MK_Universe_Color(&ctx->game.universe, ctx->game.player_ids[i]);
            assert_return_guard(pos);
            assert_return_guard(size);
            assert_return_guard(col);

            SDL_FRect rect{ pos->x, pos->y, size->x, size->y };
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, col->r, col->g, col->b, col->a));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
        }
    }

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}