#pragma once
#include "mk_common.h"
#include <cstdint>

#define MK_COLORS_MAX     UINT8_MAX
#define MK_SUBSTANCES_MAX UINT8_MAX
#define MK_ENTITIES_MAX   1024

struct MK_Substance {
    float density;
};

struct MK_Color {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

struct MK_Vec2 {
    float x;
    float y;
};

struct MK_Mass {
    float invMass;  // 0 = immovable
};

struct MK_Health {
    int health;
    int maxHealth;
};

#define MK_E_COLOR     0x0001
#define MK_E_SUBSTANCE 0x0002
#define MK_E_POSITION  0x0004
#define MK_E_SIZE      0x0008
#define MK_E_MASS      0x0010
#define MK_E_HEALTH    0x0020

struct MK_Universe {
    // Catalog
    bool         c_color_exists     [MK_COLORS_MAX];  // TODO: Bit map?
    MK_Color     c_color            [MK_COLORS_MAX];
    bool         c_substance_exists [MK_SUBSTANCES_MAX];  // TODO: Bit map?
    MK_Substance c_substance        [MK_SUBSTANCES_MAX];

    // Atoms
    uint16_t   e_exists    [MK_ENTITIES_MAX];  // bitmap
    uint8_t    e_color     [MK_ENTITIES_MAX];
    uint8_t    e_substance [MK_ENTITIES_MAX];
    MK_Vec2    e_position  [MK_ENTITIES_MAX];
    MK_Vec2    e_size      [MK_ENTITIES_MAX];
    MK_Mass    e_mass      [MK_ENTITIES_MAX];
    MK_Health  e_health    [MK_ENTITIES_MAX];
};

uint8_t    MK_Color_Create (MK_Universe *universe, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
MK_Color * MK_Color_Find   (MK_Universe *universe, uint8_t id);

uint8_t        MK_Substance_Create (MK_Universe *universe, float density);
MK_Substance * MK_Substance_Find   (MK_Universe *universe, uint8_t id);

int            MK_Universe_Init        (MK_Universe *universe);
int            MK_Universe_Create      (MK_Universe *universe, int *id, uint16_t flags);
uint8_t      * MK_Universe_ColorId     (MK_Universe *universe, int id);
uint8_t      * MK_Universe_SubstanceId (MK_Universe *universe, int id);
MK_Color     * MK_Universe_Color       (MK_Universe *universe, int id);
MK_Substance * MK_Universe_Substance   (MK_Universe *universe, int id);
MK_Vec2      * MK_Universe_Position    (MK_Universe *universe, int id);
MK_Vec2      * MK_Universe_Size        (MK_Universe *universe, int id);
MK_Mass      * MK_Universe_Mass        (MK_Universe *universe, int id);
MK_Health    * MK_Universe_Health      (MK_Universe *universe, int id);
