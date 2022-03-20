#pragma once
#include <SDL2/SDL.h>

#define MK_SUCCESS 0

#define mk_unused(x) ((void)x)

enum {
    MK_ERR_ASSERT = SDL_LOG_CATEGORY_CUSTOM,
    MK_ERR_ASSERT_SDL,
};

#define assert_return(expr) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_ERR_ASSERT, "[MK_ASSERT] %s %s:%d\n", #expr, __FILE__, __LINE__); \
            return; \
        } \
    } while(0);

#define assert_return_err(expr) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_ERR_ASSERT, "[MK_ASSERT] %s %s:%d\n", #expr, __FILE__, __LINE__); \
            return MK_ERR_ASSERT; \
        } \
    } while(0);

#define assert_sdl_return(expr) \
    do { \
        if (!(expr)) { \
            const char *str = SDL_GetError(); \
            SDL_LogCritical(MK_ERR_ASSERT_SDL, "[SDL_ASSERT] %s %s:%d %s\n", #expr, __FILE__, __LINE__, str); \
            return; \
        } \
    } while(0);

#define assert_sdl_return_err(expr) \
    do { \
        if (!(expr)) { \
            const char *str = SDL_GetError(); \
            SDL_LogCritical(MK_ERR_ASSERT_SDL, "[SDL_ASSERT] %s %s:%d %s\n", #expr, __FILE__, __LINE__, str); \
            return MK_ERR_ASSERT_SDL; \
        } \
    } while(0);