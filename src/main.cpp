#pragma warning(disable:4548)  // expression before comma has no effect
#pragma warning(disable:4710)  // function not inlined
#pragma warning(disable:4711)  // selected for automatic inline expansion (informational)
#pragma warning(disable:4820)  // bytes padding added after data member
#pragma warning(disable:5045)  // "Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified"
#define _DEBUG 1

#include "helpers.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

void MK_LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    mk_unused(userdata);
    printf("cat=%d, pri=%d, %s\n", category, priority, message);
}

struct MK_MemArena {
    size_t offset;           // bytes used
    char data[1024 * 1024];  // 1 MB hard-coded buffer (may want to make this dynamically grow)
};

void *MK_MemArena_Alloc(MK_MemArena *arena, size_t bytes) {
    if (arena->offset + bytes <= sizeof(arena->data)) {
        void *ptr = arena->data + arena->offset;
        arena->offset += bytes;
        return ptr;
    }
    assert_return_zero(("arena out of memory", 0));
}

void MK_MemArena_Clear(MK_MemArena *arena) {
    assert_return(arena);
    memset(arena->data, 0, sizeof(arena->data));
    arena->offset = 0;
}

struct MK_Context {
    SDL_Window *win;
    SDL_Renderer *rndr;
    bool quit;
    MK_MemArena *temp;           // pointer to currently active temp storage
    MK_MemArena tempBuffers[2];  // zeroed every other frame (double buffered)
    int x, y;
};

struct MK_Event_Move {
    int x;
    int y;
};

enum {
    MK_EVENT_INVALID,
    MK_EVENT_QUIT,
    MK_EVENT_MOVE,

    MK_EVENT_COUNT
};

int MK_RegisterCustomEvents() {
    assert_return_err_sdl(SDL_RegisterEvents(MK_EVENT_COUNT) != (Uint32)-1);
    return MK_SUCCESS;
}

int MK_PushEvent(int type, void *data) {
    SDL_UserEvent evt{};
    evt.type = SDL_USEREVENT;
    evt.code = type;
    evt.data1 = data;
    assert_return_err_sdl(SDL_PushEvent((SDL_Event *)&evt) >= 0);
    return MK_SUCCESS;
}

int MK_PushQuit() {
    assert_return_err_misc(!MK_PushEvent(MK_EVENT_QUIT, 0));
    return MK_SUCCESS;
}

int MK_PushMove(MK_Context *ctx, int x, int y) {
    MK_Event_Move *move = (MK_Event_Move *)MK_MemArena_Alloc(ctx->temp, sizeof(*move));
    assert_return_err_misc(move);
    move->x = x;
    move->y = y;
    assert_return_err_misc(!MK_PushEvent(MK_EVENT_MOVE, move));
    return MK_SUCCESS;
}

int MK_HandleEvent(MK_Context *ctx, int type, void *data) {
    assert_return_err_misc(type);
    printf("type: %d", type);
    switch (type) {
        case MK_EVENT_QUIT: {
            ctx->quit = true;
            break;
        }
        case MK_EVENT_MOVE: {
            MK_Event_Move *move = (MK_Event_Move *)data;
            assert_return_err_misc(move);
            printf(" move: %d %d", move->x, move->y);
            ctx->x += move->x * 5;
            ctx->y += move->y * 5;
            break;
        }
    }
    putchar('\n');
    return MK_SUCCESS;
}

int MK_PollSDLEvents(MK_Context *ctx) {

    const Uint8 *keybd = SDL_GetKeyboardState(0);

    int x = -keybd[SDL_SCANCODE_A] + keybd[SDL_SCANCODE_D];
    int y = -keybd[SDL_SCANCODE_W] + keybd[SDL_SCANCODE_S];
    if (x || y) {
        assert_return_err_misc(!MK_PushMove(ctx, x, y));
    }

    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
            case SDL_QUIT: {
                assert_return_err_misc(!MK_PushQuit());
                break;
            }
            case SDL_KEYDOWN: {
                #pragma warning(push)
                #pragma warning(disable:4061 4062)  // case not explicitly handled by switch statement
                switch (evt.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: {
                        assert_return_err_misc(!MK_PushQuit());
                        break;
                    }
                }
                #pragma warning(pop)
                break;
            }
            case SDL_USEREVENT: {
                assert_return_err_misc(!MK_HandleEvent(ctx, evt.user.code, evt.user.data1));
                break;
            }
        }
    }
    return MK_SUCCESS;
}

int MK_DrawScene(MK_Context *ctx) {
    assert_return_err_misc(ctx->rndr);

    assert_return_err_sdl(!SDL_SetRenderDrawColor(ctx->rndr, 0, 0, 0, 255));
    assert_return_err_sdl(!SDL_RenderClear(ctx->rndr));

    SDL_Rect rect{ ctx->x, ctx->y, 20, 20 };
    assert_return_err_sdl(!SDL_SetRenderDrawColor(ctx->rndr, 255, 0, 0, 255));
    assert_return_err_sdl(!SDL_RenderFillRect(ctx->rndr, &rect));

    SDL_RenderPresent(ctx->rndr);
    return MK_SUCCESS;
}

int MK_Run(MK_Context *ctx) {
    assert_return_err_misc(ctx);

    SDL_LogSetOutputFunction(MK_LogOutputFunction, 0);
    assert_return_err_sdl(!SDL_Init(SDL_INIT_EVERYTHING));
    assert_return_err_misc(!MK_RegisterCustomEvents());

    ctx->win = SDL_CreateWindow("Milk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    assert_return_err_sdl(ctx->win);

    ctx->rndr = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert_return_err_sdl(ctx->rndr);

    while (!ctx->quit) {
        assert_return_err_misc(!MK_PollSDLEvents(ctx));
        if (ctx->quit) break;

        assert_return_err_misc(!MK_DrawScene(ctx));

        // Swap temp storage
        ctx->temp = (ctx->temp == &ctx->tempBuffers[0]) ? &ctx->tempBuffers[1] : &ctx->tempBuffers[0];
        MK_MemArena_Clear(ctx->temp);
    }

    return MK_SUCCESS;
}

int main(int argc, char *argv[]) {
    mk_unused(argc);
    mk_unused(argv);

    MK_Context *ctx = (MK_Context *)calloc(1, sizeof(*ctx));
    int err = MK_Run(ctx);
    if (ctx) {
        SDL_DestroyRenderer(ctx->rndr);
        SDL_DestroyWindow(ctx->win);
        SDL_Quit();
        free(ctx);
    }

#if _DEBUG
    if (err) getchar();
#endif
    return err;
}