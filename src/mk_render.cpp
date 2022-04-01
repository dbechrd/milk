#include "mk_context.h"
#include "mk_render.h"

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 0, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));

    for (int i = 0; i < MK_PLAYERSLOT_MAX; i++) {
        if (ctx->game.universe.e_exists[i]) {
            MK_EntityID playerId = ctx->game.player_ids[i];
            MK_Vec2 *e_pos = &ctx->game.universe.e_position[playerId];
            MK_Vec2 *e_siz = &ctx->game.universe.e_size[playerId];
            MK_ColorID *e_col = &ctx->game.universe.e_color[playerId];
            MK_Color *col = &ctx->game.universe.c_color[*e_col];

            SDL_FRect rect{ e_pos->x, e_pos->y, e_siz->x, e_siz->y };
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, col->r, col->g, col->b, col->a));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
        }
    }

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}