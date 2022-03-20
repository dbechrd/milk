#pragma warning(disable:4711)  // selected for automatic inline expansion (informational)
#pragma warning(disable:4820)  // bytes padding added after data member
#pragma warning(disable:5045)  // "Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified"
#define _DEBUG 1

#include "helpers.h"
#include <SDL2/SDL.h>
#include <stdio.h>

void MK_LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    mk_unused(userdata);
    printf("cat=%d, pri=%d, %s\n", category, priority, message);
}

struct MK_Context {
    SDL_Window *win;
    SDL_Renderer *rndr;
    bool quit;
};

enum {
    MK_EVENT_INVALID,
    MK_EVENT_QUIT,
    MK_EVENT_DATA,

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

int MK_HandleEvent(MK_Context *ctx, int type, void *data) {
    assert_return_err_misc(type);
    switch (type) {
        case MK_EVENT_QUIT: {
            ctx->quit = true;
            break;
        }
        case MK_EVENT_DATA: {
            assert_return_err_misc(data);
            break;
        }
    }
    return MK_SUCCESS;
}

int MK_PollSDLEvents(MK_Context *ctx) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
            case SDL_QUIT: {
                assert_return_err_misc(!MK_PushEvent(MK_EVENT_QUIT, 0));
                break;
            }
            case SDL_KEYDOWN: {
                #pragma warning(push)
                #pragma warning(disable:4061 4062)  // case not explicitly handled by switch statement
                switch (evt.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: {
                        assert_return_err_misc(!MK_PushEvent(MK_EVENT_QUIT, 0));
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

    SDL_Rect rect{ 0, 0, 20, 20 };
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

    ctx->win = SDL_CreateWindow("homogeneity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    assert_return_err_sdl(ctx->win);

    ctx->rndr = SDL_CreateRenderer(ctx->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert_return_err_sdl(ctx->rndr);

    while (!ctx->quit) {
        assert_return_err_misc(!MK_PollSDLEvents(ctx));
        if (ctx->quit) break;

        assert_return_err_misc(!MK_DrawScene(ctx));
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