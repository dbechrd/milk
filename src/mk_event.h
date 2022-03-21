#pragma once
#include "mk_common.h"

struct MK_Context;

enum {
    MK_EVENT_INVALID,
    MK_EVENT_QUIT,
    MK_EVENT_MOVE,

    MK_EVENT_COUNT
};

struct MK_Event_Move {
    int x;
    int y;
};

int MK_RegisterCustomEvents ();
int MK_PushEvent            (int type, void *data);
int MK_PushQuit             ();
int MK_PushMove             (MK_Context *ctx, int x, int y);
int MK_HandleEvent          (MK_Context *ctx, int type, void *data);
int MK_PollSDLEvents        (MK_Context *ctx);