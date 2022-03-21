#pragma once
#include "mk_common.h"

struct MK_Attr_Position {
    //int id;
    SDL_Point position;
};

struct MK_Attr_Color {
    SDL_Color color;
};

struct MK_Attr_Health {
    //int id;
    int health;
};

#define MK_ENTITIES_MAX 1024

struct MK_Universe {
    MK_Attr_Position position [MK_ENTITIES_MAX];
    MK_Attr_Color    color    [MK_ENTITIES_MAX];
    MK_Attr_Health   health   [MK_ENTITIES_MAX];
    bool             exists   [MK_ENTITIES_MAX];
};

int MK_Universe_Create   (MK_Universe *universe, int *id);
int MK_Universe_GetPos   (MK_Universe *universe, int id, MK_Attr_Position *position);
int MK_Universe_SetPos   (MK_Universe *universe, int id, const MK_Attr_Position *position);
int MK_Universe_GetColor (MK_Universe *universe, int id, MK_Attr_Color *color);
int MK_Universe_SetColor (MK_Universe *universe, int id, const MK_Attr_Color *color);