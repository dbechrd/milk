#pragma once
#include "mk_common.h"

struct MK_Context;

int  MK_App_Init     (MK_Context **ctxRef);
int  MK_App_Run      (MK_Context *ctx);
void MK_App_Shutdown (MK_Context **ctxRef);