#pragma once
#include "mk_common.h"

#define MK_COLORID_MAX      UINT8_MAX
#define MK_SUBSTANCEID_MAX  UINT8_MAX
#define MK_ENTITYID_MAX     500 //UINT16_MAX
typedef uint8_t MK_ColorID;
typedef uint8_t MK_SubstanceID;
typedef uint16_t MK_EntityID;

#define MK_GRID_W        1600
#define MK_CELLS_PER_ROW 32
#define MK_CELL_W        (MK_GRID_W / MK_CELLS_PER_ROW)
#define MK_CELL_MAX      (MK_CELLS_PER_ROW * MK_CELLS_PER_ROW)
typedef uint16_t MK_CellIndex;

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

struct MK_SpatialBounds {
    MK_CellIndex x0, y0;
    MK_CellIndex x1, y1;
};

struct MK_ChainEntry {
    MK_EntityID offset;  // index of first chain entry
    MK_EntityID length;  // length of chain
};

#define MK_E_COLOR     (1 << 0)
#define MK_E_SUBSTANCE (1 << 1)
#define MK_E_POSITION  (1 << 2)
#define MK_E_VELOCITY  (1 << 3)
#define MK_E_SIZE      (1 << 4)
#define MK_E_MASS      (1 << 5)
#define MK_E_HEALTH    (1 << 6)

struct MK_Universe {
    // Catalog
    MK_Color     c_color       [MK_COLORID_MAX];
    MK_Substance c_substance   [MK_SUBSTANCEID_MAX];

    // Atoms
    uint16_t         e_exists    [MK_ENTITYID_MAX];  // bitmap
    MK_ColorID       e_color     [MK_ENTITYID_MAX];
    MK_SubstanceID   e_substance [MK_ENTITYID_MAX];
    MK_Vec2          e_position  [MK_ENTITYID_MAX];
    MK_Vec2          e_velocity  [MK_ENTITYID_MAX];
    MK_Vec2          e_size      [MK_ENTITYID_MAX];
    MK_Mass          e_mass      [MK_ENTITYID_MAX];
    MK_Health        e_health    [MK_ENTITYID_MAX];
    MK_SpatialBounds e_cell      [MK_ENTITYID_MAX];  // 16x16 spatial grid, 0 top left

    // Spatial grid
    MK_ChainEntry  chain_index [MK_CELL_MAX];      // cell -> chain offset
    MK_EntityID    chain_table [MK_ENTITYID_MAX];  // contiguous, dense chain storage
};

int MK_Universe_Init   (MK_Universe *universe);
int MK_Universe_Create (MK_Universe *universe, MK_EntityID *id, uint16_t flags);

MK_ColorID     MK_Color_Create     (MK_Universe *universe, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
MK_SubstanceID MK_Substance_Create (MK_Universe *universe, float density);