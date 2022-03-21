#pragma once
#include "mk_common.h"

struct MK_Attr_Position {
    //int id;
    int x, y;
};

struct MK_Attr_Health {
    //int id;
    int health;
};

#define MK_ENTITIES_MAX 1024

struct MK_Universe {
    MK_Attr_Position position [MK_ENTITIES_MAX];
    MK_Attr_Health   health   [MK_ENTITIES_MAX];
    bool             exists   [MK_ENTITIES_MAX];
};

int MK_Universe_Create(MK_Universe *universe, int *id);
int MK_Universe_Move(MK_Universe *universe, int id, int x, int y);