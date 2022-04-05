#include "mk_context.h"
#include "mk_render.h"

int MK_DrawRect(MK_Context *ctx, SDL_FRect rect, SDL_Color color) {
    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, color.r, color.g, color.b, color.a));
    succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
    return MK_SUCCESS;
}

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 100, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));

    for (MK_EntityID e_id = 0; e_id < MK_ENTITYID_MAX; e_id++) {
        if (ctx->game.universe.e_exists[e_id]) {
            MK_Vec2 *e_pos = &ctx->game.universe.e_position[e_id];
            MK_Vec2 *e_siz = &ctx->game.universe.e_size[e_id];
            MK_ColorID *e_col = &ctx->game.universe.e_color[e_id];
            MK_Color *col = &ctx->game.universe.c_color[*e_col];

            SDL_FRect rect{ e_pos->x, e_pos->y, e_siz->x, e_siz->y };
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, col->r, col->g, col->b, col->a));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
        }
    }

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}