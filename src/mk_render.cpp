#include "mk_context.h"
#include "mk_render.h"

int MK_DrawRect(MK_Context *ctx, SDL_FRect rect, SDL_Color color) {
    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, color.r, color.g, color.b, color.a));
    succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
    return MK_SUCCESS;
}

int MK_DrawTriangle(MK_Context *ctx, MK_Vec2 pos, MK_Vec2 siz, MK_Color col) {
    SDL_Vertex vertices[3] = {};
    vertices[0].position = { pos.x + siz.x, pos.y + siz.y };
    vertices[0].color = *(SDL_Color *)&col;
    vertices[1].position = { pos.x + siz.x / 2, pos.y };
    vertices[1].color = *(SDL_Color *)&col;
    vertices[2].position = { pos.x, pos.y + siz.y };
    vertices[2].color = *(SDL_Color *)&col;

    succeed_or_return_expr_sdl(SDL_RenderGeometry(ctx->rndr, 0, vertices, mk_array_count(vertices), 0, 0));
    return MK_SUCCESS;
}

int MK_DrawScene(MK_Context *ctx) {
    assert_return_guard(ctx->rndr);

    succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 0, 100, 0, 255));
    succeed_or_return_expr_sdl(SDL_RenderClear(ctx->rndr));
    succeed_or_return_expr_sdl(SDL_SetRenderDrawBlendMode(ctx->rndr, SDL_BLENDMODE_BLEND));

    static float wobble = 0.0f;
#if 0
    wobble = sinf((float)SDL_GetTicks() / 100.0f) * 10.0f;
#endif

    if (ctx->game.debug_show_spatial_grid_cells) {
        for (MK_EntityID e_id = 0; e_id < MK_ENTITYID_MAX; e_id++) {
            if (!ctx->game.universe.e_exists[e_id]) continue;

            MK_SpatialBounds *bounds = &ctx->game.universe.e_cell[e_id];
            SDL_FRect bounds_rect{
                (float)(bounds->x0 * MK_CELL_W),
                (float)(bounds->y0 * MK_CELL_W),
                (float)((bounds->x1 - bounds->x0 + 1) * MK_CELL_W),
                (float)((bounds->y1 - bounds->y0 + 1) * MK_CELL_W)
            };
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 255, 205, 56, 50));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &bounds_rect));
        }
    }

#if 0
    MK_EntityID playerId = ctx->game.player_ids[0];
    for (MK_CellIndex cell = 0; cell < MK_CELL_MAX; cell++) {
        MK_ChainEntry *entry = &ctx->game.universe.chain_index[cell];
        MK_Color color{ 0, 0, 0, 150 };
        for (int i = 0; i < entry->length; i++) {
            if (ctx->game.universe.chain_table[entry->offset + i] == ctx->game.player_ids[0]) {
                color.r = 255;
            } else if (ctx->game.universe.chain_table[entry->offset + i] == ctx->game.player_ids[1]) {
                color.g = 255;
            } else if (ctx->game.universe.chain_table[entry->offset + i] == ctx->game.player_ids[2]) {
                color.b = 255;
            }
        }
        MK_CellIndex x = cell % MK_CELLS_PER_ROW;
        MK_CellIndex y = cell / MK_CELLS_PER_ROW;
        SDL_FRect bounds_rect{
            (float)(x)*MK_CELL_W,
            (float)(y)*MK_CELL_W,
            (float)(MK_CELL_W),
            (float)(MK_CELL_W)
        };
        succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, color.r, color.g, color.b, color.a));
        succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &bounds_rect));
    }
#endif

    for (MK_EntityID e_id = 0; e_id < MK_ENTITYID_MAX; e_id++) {
        if (!ctx->game.universe.e_exists[e_id]) continue;

        MK_Vec2 *e_pos = &ctx->game.universe.e_position[e_id];
        MK_Vec2 *e_siz = &ctx->game.universe.e_size[e_id];
        MK_ColorID *e_col = &ctx->game.universe.e_color[e_id];
        MK_Color *col = &ctx->game.universe.c_color[*e_col];

        SDL_FRect rect{ e_pos->x, e_pos->y, e_siz->x, e_siz->y };
#if 0
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, 255, 0, 0, 127));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));

            MK_Vec2 pos = *e_pos;
            pos.x += wobble;
            succeed_or_return_expr(MK_DrawTriangle(ctx, pos, *e_siz, *col));
#else
            succeed_or_return_expr_sdl(SDL_SetRenderDrawColor(ctx->rndr, col->r, col->g, col->b, col->a));
            succeed_or_return_expr_sdl(SDL_RenderFillRectF(ctx->rndr, &rect));
#endif
    }

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}