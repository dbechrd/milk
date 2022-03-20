#pragma once
#include <SDL2/SDL.h>

#define mk_unused(x) ((void)x)

// Application-specific SDL log categories
enum {
    MK_LOG_ASSERT = SDL_LOG_CATEGORY_CUSTOM
};

// Application-specific error codes
enum {
    MK_SUCCESS = 0,
    MK_ERR_MISC = -1,
    MK_ERR_SDL = -2,
};

// Macro generator, not meant to be called directly from user code
#define assert_return__generator(expr, msg, ret) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s\n  %s:%d\n  %s\n", #expr, __FILE__, __LINE__, msg); \
            ret; \
        } \
    } while(0)

// If assert fails, log and return from caller
#define assert_return(expr) assert_return__generator(expr, "generic assert", return)

// If assert fails, log and return zero from caller
#define assert_return_zero(expr) assert_return__generator((expr), "generic assert", return 0)

// If assert fails, log and return false from caller
#define assert_return_false(expr) assert_return__generator((expr), "generic assert", return false)

// If assert fails, log and return provided error code from caller
#define assert_return_err(expr, err) assert_return__generator((expr), "generic assert", return (err))

// If assert fails, log and return ERR_MISC error code from caller
#define assert_return_err_misc(expr) assert_return__generator((expr), "misc error", return MK_ERR_MISC)

// If assert fails, log (including SDL error message) and return ERR_SDL from caller
#define assert_return_err_sdl(expr) assert_return__generator((expr), SDL_GetError(), return MK_ERR_SDL)