#pragma once
#pragma warning(disable:4548)  // expression before comma has no effect
#pragma warning(disable:4710)  // function not inlined
#pragma warning(disable:4711)  // selected for automatic inline expansion (informational)
#pragma warning(disable:4820)  // bytes padding added after data member
#pragma warning(disable:5045)  // "Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified"

#include <SDL2/SDL.h>
#include <cinttypes>
//#include <intrin.h>

#define mk_unused(x) ((void)x)
#define mk_min(x, y) ((x) <= (y) ? (x) : (y))
#define mk_max(x, y) ((x) >= (y) ? (x) : (y))
#define mk_clamp(x, a, b) mk_min(mk_max(a, x), b)
#define mk_array_count(a) (sizeof(a)/sizeof((a)[0]))

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

const char *mk_err_string(int err);

// Macro generator, not meant to be called directly from user code
#define mk_log_info(msg, ...) \
    SDL_Log("[mk_log_info:%" PRIu64 "] %s:%d\n  " msg "\n", SDL_GetTicks64(), __FILE__, __LINE__, __VA_ARGS__);

// Macro generator, not meant to be called directly from user code
#define assert_abort(expr) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s:%d\n  %s\n", __FILE__, __LINE__, #expr); \
            __debugbreak(); \
            exit(-1); \
        } \
    } while(0)

// Macro generator, not meant to be called directly from user code
#define assert_return__generator(expr, ret) \
    do { \
        if (!(expr)) { \
            ret; \
        } \
    } while(0)

// If assert fails, log and return from caller
#define assert_return(expr) assert_return__generator(expr, return)

// If assert fails, log and return zero from caller
#define assert_return_zero(expr) assert_return__generator(expr, return 0)

// If assert fails, log and return false from caller
#define assert_return_false(expr) assert_return__generator(expr, return false)

// If assert fails, log and return from caller
#define assert_return_value(expr, value) assert_return__generator(expr, return value)

// Macro generator, not meant to be called directly from user code
#define assert_return_log__generator(expr, msg, ret) \
    do { \
        if (!(expr)) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s:%d\n  %s\n  %s\n", __FILE__, __LINE__, #expr, (msg)); \
            __debugbreak(); \
            ret; \
        } \
    } while(0)

// If assert fails, log and return specified error code from caller
#define assert_return_err(expr, msg, err) assert_return_log__generator(expr, msg, return (err))

// If assert fails, log and return MK_ERR_GUARD_COND from caller
#define assert_return_guard(expr) assert_return_log__generator(expr, "guard condition", return MK_ERR_GUARD_COND)

// If assert fails, log (including SDL error message) and return MK_ERR_SDL from caller
#define assert_return_sdl(expr) assert_return_log__generator(expr, SDL_GetError(), return MK_ERR_SDL)

// Macro generator, not meant to be called directly from user code
#define succeed_or_return__generator(expr, msg) \
    do { \
        int err = (expr); \
        if (err < 0) { \
            SDL_LogCritical(MK_LOG_ASSERT, "[MK_ASSERT] %s:%d\n  %s\n  [%d] %s\n", __FILE__, __LINE__, #expr, err, msg); \
            return err; \
        } \
    } while(0)

// If expr returns a negative error code, log and return error code from caller
#define succeed_or_return_expr(expr) succeed_or_return__generator(expr, mk_err_string(err))

// If expr returns a negative error code, log (including SDL error message) and return error code from caller
#define succeed_or_return_expr_sdl(expr) succeed_or_return__generator(expr, SDL_GetError())
