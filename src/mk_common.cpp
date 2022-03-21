#include "mk_common.h"

const char *mk_err_string(int err) {
    switch (err) {
        case MK_SUCCESS:        return "MK_SUCCESS";
        case MK_ERR_SDL:        return "MK_ERR_SDL";
        case MK_ERR_GUARD_COND: return "MK_ERR_GUARD_COND";
        case MK_ERR_BAD_ALLOC:  return "MK_ERR_BAD_ALLOC";
        default: return "<mk_err_uknown>";
    }
}