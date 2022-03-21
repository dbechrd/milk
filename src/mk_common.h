#pragma once
#pragma warning(disable:4548)  // expression before comma has no effect
#pragma warning(disable:4710)  // function not inlined
#pragma warning(disable:4711)  // selected for automatic inline expansion (informational)
#pragma warning(disable:4820)  // bytes padding added after data member
#pragma warning(disable:5045)  // "Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified"

#include <SDL2/SDL.h>

#define mk_unused(x) ((void)x)

// Application-specific SDL log categories
enum {
    MK_LOG_ASSERT = SDL_LOG_CATEGORY_CUSTOM,
    MK_LOG_EVENT
};

// Application-specific error codes
enum {
    MK_SUCCESS        =  0,
    MK_ERR_SDL        = -1,
    MK_ERR_GUARD_COND = -2,
    MK_ERR_BAD_ALLOC  = -3,
};

// Macro generator, not meant to be called directly from user code
#define assert_return__generator(expr, msg, ret) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s:%d\n  %s\n  %s\n", __FILE__, __LINE__, #expr, msg); \
            ret; \
        } \
    } while(0)

// If assert fails, log and return from caller
#define assert_return(expr) assert_return__generator(expr, "assert failed", return)

// If assert fails, log and return zero from caller
#define assert_return_zero(expr) assert_return__generator((expr), "assert failed", return 0)

// If assert fails, log and return false from caller
#define assert_return_false(expr) assert_return__generator((expr), "assert failed", return false)

// If assert fails, log and return specified error code from caller
#define assert_return_err(expr, err) assert_return__generator((expr), "assert failed", return (err))

// If assert fails, log and return specified error code from caller
#define assert_return_guard(expr) assert_return__generator((expr), "guard condition failed", return MK_ERR_GUARD_COND)

// If assert fails, log (including SDL error message) and return MK_ERR_SDL from caller
#define assert_return_sdl(expr) assert_return__generator((expr), SDL_GetError(), return MK_ERR_SDL)

// Macro generator, not meant to be called directly from user code
#define succeed_or_return__generator(expr, msg) \
    do { \
        int err = (expr); \
        if (err < 0) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s:%d\n  %s\n  [%d] %s\n", __FILE__, __LINE__, #expr, err, msg); \
            return err; \
        } \
    } while(0)

// Fails if expr returns a negative error code
#define succeed_or_return_expr(expr) succeed_or_return__generator((expr), "assert failed")

// Fails if expr returns a negative error code and logs an SDL error
#define succeed_or_return_expr_sdl(expr) succeed_or_return__generator((expr), SDL_GetError())