#include "mk_app.h"
#include "mk_context.h"
#include "mk_event.h"
#include "mk_log.h"
#include "mk_memarena.h"
#include "mk_render.h"

int MK_App_Init(MK_Context **ctxRef) {
    assert_return_guard(ctxRef);

    *ctxRef = (MK_Context *)calloc(1, sizeof(**ctxRef));
    MK_Context *ctx = *ctxRef;
    assert_return_err(ctx, "Failed to allocate main app context", MK_ERR_BAD_ALLOC);

    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

    SDL_LogSetOutputFunction(MK_LogOutputFunction, 0);
    succeed_or_return_expr_sdl(SDL_Init(SDL_INIT_EVERYTHING));
    succeed_or_return_expr(MK_RegisterCustomEvents());

    ctx->win = SDL_CreateWindow("Milk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    assert_return_sdl(ctx->win);

    ctx->rndr = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert_return_sdl(ctx->rndr);

    return MK_SUCCESS;
}

int MK_App_Run(MK_Context *ctx) {
    assert_return_guard(ctx);

    succeed_or_return_expr(MK_Game_Init(&ctx->game));

    while (!ctx->quit) {
        succeed_or_return_expr(MK_PollSDLEvents(ctx));
        if (ctx->quit) break;

        succeed_or_return_expr(MK_Game_Simulate(&ctx->game, 1.0f / 60.0f));

        succeed_or_return_expr(MK_DrawScene(ctx));

        // Swap temp storage
        ctx->temp = (ctx->temp == &ctx->tempBuffers[0]) ? &ctx->tempBuffers[1] : &ctx->tempBuffers[0];
        MK_MemArena_Clear(ctx->temp);
    }

    return MK_SUCCESS;
}

void MK_App_Shutdown(MK_Context **ctxRef) {
    assert_return(ctxRef);

    MK_Context *ctx = *ctxRef;
    assert_return(ctx);

    SDL_DestroyRenderer(ctx->rndr);
    SDL_DestroyWindow(ctx->win);
    SDL_Quit();
    free(ctx);
    *ctxRef = 0;
}