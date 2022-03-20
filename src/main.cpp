#pragma warning(disable:5045)  // "Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified"
#pragma warning(disable:4711)  // selected for automatic inline expansion (informational)

#include "helpers.h"
#include <SDL2/SDL.h>
#include <stdio.h>

void MK_LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    mk_unused(userdata);
    printf("cat=%d, prior=%d, %s\n", category, priority, message);
}

enum {
    MK_EVENT_QUIT,

    MK_EVENT_COUNT
};

int RegisterCustomEvents() {
    assert_sdl_return_err(SDL_RegisterEvents(MK_EVENT_COUNT) != (Uint32)-1);
    return MK_SUCCESS;
}

void MK_PushEvent(Sint32 type, void *data) {
    SDL_UserEvent evt{};
    evt.type = SDL_USEREVENT;
    evt.code = type;
    evt.data1 = data;
    assert_sdl_return(SDL_PushEvent((SDL_Event *)&evt) >= 0);
}

void HandleKeyboardEvent(bool *quit) {
    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
            case SDL_QUIT: {
                MK_PushEvent(MK_EVENT_QUIT, 0);
                break;
            }
            case SDL_KEYDOWN: {
                #pragma warning(push)
                #pragma warning(disable:4061 4062)  // case not explicitly handled by switch statement
                switch (evt.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: {
                        MK_PushEvent(MK_EVENT_QUIT, 0);
                        break;
                    }
                }
                #pragma warning(pop)
                break;
            }
            case SDL_USEREVENT: {
                switch (evt.user.code) {
                    case MK_EVENT_QUIT: {
                        *quit = true;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void DrawScene(SDL_Renderer *renderer) {
    assert_return(renderer);

    assert_sdl_return(!SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255));
    assert_sdl_return(!SDL_RenderClear(renderer));

    SDL_Rect rect{ 0, 0, 20, 20 };
    assert_sdl_return(!SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255));
    assert_sdl_return(!SDL_RenderFillRect(renderer, &rect));

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    mk_unused(argc);
    mk_unused(argv);

    SDL_LogSetOutputFunction(MK_LogOutputFunction, 0);

    assert_sdl_return_err(!SDL_Init(SDL_INIT_EVERYTHING));

    SDL_Window *window = SDL_CreateWindow("homogeneity", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, 0);
    assert_sdl_return_err(window);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    assert_sdl_return_err(renderer);

    assert_return_err(!RegisterCustomEvents());

    bool quit = false;
    while (!quit) {
        HandleKeyboardEvent(&quit);
        if (quit) break;

        DrawScene(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}