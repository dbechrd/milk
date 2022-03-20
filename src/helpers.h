#pragma once
#include <SDL2/SDL.h>

#define mk_unused(x) ((void)x)

enum {
    MK_LOG_ASSERT = SDL_LOG_CATEGORY_CUSTOM,
    MK_LOG_ASSERT_SDL,
};

enum {
    MK_SUCCESS = 0,
    MK_ERR_MISC = -1,
    MK_ERR_SDL = -2,
};

#define assert_return__generator(expr, msg, ret) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s\n  %s:%d\n  %s\n", #expr, __FILE__, __LINE__, msg); \
            ret; \
        } \
    } while(0)

#define assert_return(expr) assert_return__generator(expr, "generic assert", return)
#define assert_return_zero(expr) assert_return__generator((expr), "generic assert", return 0)
#define assert_return_false(expr) assert_return__generator((expr), "generic assert", return false)
#define assert_return_err(expr, err) assert_return__generator((expr), "generic assert", return (err))
#define assert_return_err_misc(expr) assert_return_err((expr), MK_ERR_MISC)
#define assert_return_err_sdl(expr) assert_return_err((expr), MK_ERR_SDL)
//#define assert_success_return_code(expr) assert_return__generator(!(expr), "assert failed", return (expr))