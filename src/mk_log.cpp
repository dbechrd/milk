#include "mk_common.h"
#include <stdio.h>

void MK_LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message) {
    mk_unused(userdata);
    mk_unused(category);
    mk_unused(priority);
    //printf("cat=%d, pri=%d, %s\n", category, priority, message);
    printf("%s\n", message);
}