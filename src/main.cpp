#include "mk_app.h"
#include "mk_common.h"

int main(int argc, char *argv[]) {
    mk_unused(argc);
    mk_unused(argv);

    MK_Context *ctx = 0;
    int err = MK_App_Init(&ctx);
    if (!err) {
        err = MK_App_Run(ctx);
    }
    MK_App_Shutdown(&ctx);

    if (err) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "See log for details.", 0);
    }
    return err;
}
