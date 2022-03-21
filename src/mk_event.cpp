#include "mk_context.h"
#include "mk_event.h"
#include "mk_game.h"
#include "mk_memarena.h"

int MK_RegisterCustomEvents() {
    assert_return_sdl(SDL_RegisterEvents(MK_EVENT_COUNT) != (Uint32)-1);
    return MK_SUCCESS;
}

int MK_PushEvent(int type, void *data) {
    SDL_UserEvent evt{};
    evt.type = SDL_USEREVENT;
    evt.code = type;
    evt.data1 = data;
    succeed_or_return_expr_sdl(SDL_PushEvent((SDL_Event *)&evt));
    return MK_SUCCESS;
}

int MK_PushQuit() {
    succeed_or_return_expr(MK_PushEvent(MK_EVENT_QUIT, 0));
    return MK_SUCCESS;
}

int MK_PushMove(MK_Context *ctx, int x, int y) {
    MK_Event_Move *move = (MK_Event_Move *)MK_MemArena_Alloc(ctx->temp, sizeof(*move));
    assert_return_err(move, "Failed to allocate move event", MK_ERR_BAD_ALLOC);
    move->x = x;
    move->y = y;
    succeed_or_return_expr(MK_PushEvent(MK_EVENT_MOVE, move));
    return MK_SUCCESS;
}

int MK_HandleEvent(MK_Context *ctx, int type, void *data) {
    assert_return_guard(type);
    SDL_LogVerbose(MK_LOG_EVENT, "type: %d", type);
    switch (type) {
        case MK_EVENT_QUIT: {
            ctx->quit = true;
            break;
        }
        case MK_EVENT_MOVE: {
            MK_Event_Move *move = (MK_Event_Move *)data;
            assert_return_guard(move);
            SDL_LogVerbose(MK_LOG_EVENT, " move: %d %d", move->x, move->y);
            MK_Game_PlayerMove(&ctx->game, move->x * 5, move->y * 5);
            break;
        }
    }
    SDL_LogVerbose(MK_LOG_EVENT, "\n");
    return MK_SUCCESS;
}

int MK_PollSDLEvents(MK_Context *ctx) {

    const Uint8 *keybd = SDL_GetKeyboardState(0);

    int x = -keybd[SDL_SCANCODE_A] + keybd[SDL_SCANCODE_D];
    int y = -keybd[SDL_SCANCODE_W] + keybd[SDL_SCANCODE_S];
    if (x || y) {
        succeed_or_return_expr(MK_PushMove(ctx, x, y));
    }

    SDL_Event evt;
    while (SDL_PollEvent(&evt)) {
        switch (evt.type) {
            case SDL_QUIT: {
                succeed_or_return_expr(MK_PushQuit());
                break;
            }
            case SDL_KEYDOWN: {
                #pragma warning(push)
                #pragma warning(disable:4061 4062)  // case not explicitly handled by switch statement
                switch (evt.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE: {
                        succeed_or_return_expr(MK_PushQuit());
                        break;
                    }
                }
                #pragma warning(pop)
                break;
            }
            case SDL_USEREVENT: {
                succeed_or_return_expr(MK_HandleEvent(ctx, evt.user.code, evt.user.data1));
                break;
            }
        }
    }
    return MK_SUCCESS;
}