#include "mk_game.h"
#include <cstring>

int MK_Game_Init(MK_Game *game) {
#if _DEBUG
    //game->debug_no_gravity = true;
    //game->debug_no_physics_simulation = true;
    //game->debug_no_collision_resolution = true;
#endif

    succeed_or_return_expr(MK_Universe_Init(&game->universe));

    uint8_t color_fence_id = MK_Color_Create(&game->universe,  80,  40,   0, 255);
    uint8_t color_red_id   = MK_Color_Create(&game->universe, 255, 110, 110, 255);
    uint8_t color_green_id = MK_Color_Create(&game->universe, 110, 255, 110, 255);
    uint8_t color_blue_id  = MK_Color_Create(&game->universe, 110, 110, 255, 255);
    uint8_t color_water_id = MK_Color_Create(&game->universe,  10,  10, 170, 255);

    succeed_or_return_expr(MK_Game_WallInit(game, {         0,        0}, 1600,  20, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, {         0, 900 - 20}, 1600,  20, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, {         0,        0},   20, 900, color_fence_id, 0));
    succeed_or_return_expr(MK_Game_WallInit(game, { 1600 - 20,        0},   20, 900, color_fence_id, 0));

    uint8_t substance_water = MK_Substance_Create(&game->universe, 1.0f);
    uint8_t substance_sink = MK_Substance_Create(&game->universe, 4.0f);
    uint8_t substance_styrofoam = MK_Substance_Create(&game->universe, 0.3f);
    uint8_t substance_steel = MK_Substance_Create(&game->universe, 10.0f);
    succeed_or_return_expr(MK_Game_PhysicsBodyInit(game, &game->player_ids[0], { 100.0f, 100.0f }, 100.0f, 100.0f, color_red_id, substance_sink));
    succeed_or_return_expr(MK_Game_PhysicsBodyInit(game, 0, { 500.0f, 100.0f }, 200.0f, 150.0f, color_green_id, substance_steel));
    succeed_or_return_expr(MK_Game_PhysicsBodyInit(game, 0, { 300.0f, 200.0f }, 150.0f, 50.0f, color_blue_id, substance_styrofoam));

    float x = 400.0f;
    float y = 400.0f;
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            succeed_or_return_expr(MK_Game_PhysicsBodyInit(game, 0, { x, y }, 16.0f, 16.0f, color_water_id, substance_water));
            x += 20.0f;
        }
        x = 400.0f;
        y += 20.0f;
    }

    return MK_SUCCESS;
}

int MK_Game_WallInit(MK_Game *game, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    uint16_t flags = MK_E_COLOR | MK_E_SUBSTANCE | MK_E_POSITION | MK_E_SIZE | MK_E_MASS;
    MK_EntityID entity_id = 0;
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &entity_id, flags));

    MK_Substance *substance = &game->universe.c_substance[substance_id];

    MK_Vec2 *e_pos  = &game->universe.e_position[entity_id];
    MK_Vec2 *e_size = &game->universe.e_size[entity_id];
    MK_Mass *e_mass = &game->universe.e_mass[entity_id];

    game->universe.e_color[entity_id] = color_id;
    game->universe.e_substance[entity_id] = substance_id;

    *e_pos = position;
    e_size->x = w;
    e_size->y = h;
    float mass = substance->density * e_size->x * e_size->y;
    e_mass->invMass = mass ? 1.0f / mass : 0;
    return MK_SUCCESS;
}

int MK_Game_PhysicsBodyInit(MK_Game *game, MK_EntityID *id, MK_Vec2 position, float w, float h, uint8_t color_id, uint8_t substance_id) {
    uint16_t flags = MK_E_COLOR | MK_E_SUBSTANCE | MK_E_POSITION | MK_E_VELOCITY | MK_E_SIZE | MK_E_MASS | MK_E_HEALTH;
    MK_EntityID entity_id = 0;
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &entity_id, flags));

    MK_Substance *substance = &game->universe.c_substance[substance_id];

    MK_Vec2   *e_pos    = &game->universe.e_position[entity_id];
    MK_Vec2   *e_size   = &game->universe.e_size[entity_id];
    MK_Mass   *e_mass   = &game->universe.e_mass[entity_id];
    MK_Health *e_health = &game->universe.e_health[entity_id];

    game->universe.e_color[entity_id] = color_id;
    game->universe.e_substance[entity_id] = substance_id;

    *e_pos = position;
    e_size->x = w;
    e_size->y = h;
    float mass = substance->density * e_size->x * e_size->y;
    e_mass->invMass = mass ? 1.0f / mass : 0;
    e_health->health = 100;
    e_health->maxHealth = 100;

    if (id) *id = entity_id;
    return MK_SUCCESS;
}

static MK_CellIndex MK_Game_CalculateGridCell(MK_Game *game, float value) {
    mk_unused(game);  // TODO: Get grid/cell size from game or game->universe?
    MK_CellIndex grid_cell = (MK_CellIndex)floorf(value / MK_GRID_W * MK_CELLS_PER_ROW);
    return grid_cell;
}

static void MK_Game_UpdateSpatialBounds(MK_Game *game, MK_EntityID id) {
    MK_Vec2 *e_pos = &game->universe.e_position[id];
    MK_Vec2 *e_siz = &game->universe.e_size[id];
    MK_SpatialBounds *bounds = &game->universe.e_cell[id];
    bounds->x0 = MK_Game_CalculateGridCell(game, e_pos->x);
    bounds->y0 = MK_Game_CalculateGridCell(game, e_pos->y);
    bounds->x1 = MK_Game_CalculateGridCell(game, e_pos->x + e_siz->x);
    bounds->y1 = MK_Game_CalculateGridCell(game, e_pos->y + e_siz->y);
}

int MK_Game_UpdateSpatialGrid(MK_Game *game) {
    memset(game->universe.chain_index, 0, sizeof(game->universe.chain_index));
    memset(game->universe.chain_table, 0, sizeof(game->universe.chain_table));

    for (MK_EntityID e_id = 0; e_id < MK_ENTITYID_MAX; e_id++) {
        MK_Game_UpdateSpatialBounds(game, e_id);
        MK_SpatialBounds *bounds = &game->universe.e_cell[e_id];
        for (MK_CellIndex y = bounds->y0; y <= bounds->y1; y++) {
            for (MK_CellIndex x = bounds->x0; x <= bounds->x1; x++) {
                MK_CellIndex cell = (MK_CellIndex)(y * MK_CELLS_PER_ROW + x);
                game->universe.chain_index[cell].length++;
                if (x == MK_CELL_MAX) break;
            }
            if (y == MK_CELL_MAX) break;
        }
    }

    // TODO: Update chain_table

    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, MK_PlayerSlot player_slot, float x, float y) {
    MK_EntityID player_id = game->player_ids[player_slot];
    MK_Vec2 *e_vel = &game->universe.e_velocity[player_id];
    e_vel->x += x * 5.0f;
    e_vel->y += y * 5.0f;
    return MK_SUCCESS;
}

static int MK_Game_SimulatePhysics(MK_Game *game, float dt) {
    assert_return_zero(!game->debug_no_physics_simulation);

    for (MK_EntityID e_id = 0; e_id < MK_ENTITYID_MAX; e_id++) {
        MK_Vec2 *e_pos = &game->universe.e_position[e_id];
        MK_Vec2 *e_siz = &game->universe.e_size[e_id];
        MK_Vec2 *e_vel = &game->universe.e_velocity[e_id];
        MK_Mass *e_mas = &game->universe.e_mass[e_id];

        if (!e_mas->invMass) continue; // immovable

        if (!game->debug_no_gravity) {
            e_vel->y += 1000.0f * dt;
        }

        e_vel->x += -e_vel->x * 0.98f * dt;
        e_vel->y += -e_vel->y * 0.98f * dt;
        e_pos->x += e_vel->x * dt;
        e_pos->y += e_vel->y * dt;

        // Hard constaint to keep things in bounds
        e_pos->x = mk_clamp(e_pos->x, 0.0f, MK_GRID_W - e_siz->x);
        e_pos->y = mk_clamp(e_pos->y, 0.0f, MK_GRID_W - e_siz->y);
    }
    return MK_SUCCESS;
}

struct MK_Manifold {
    float x;
    float y;
};

static bool MK_Intersects(MK_Manifold *manifold, MK_Vec2 *pos1, MK_Vec2 *siz1, MK_Vec2 *pos2, MK_Vec2 *siz2) {
    float x_overlap = mk_min(
        (pos1->x + siz1->x) - pos2->x,
        (pos2->x + siz2->x) - pos1->x
    );

    float y_overlap = mk_min(
        (pos1->y + siz1->y) - pos2->y,
        (pos2->y + siz2->y) - pos1->y
    );

    if (x_overlap < 0 || y_overlap < 0) {
        return false;
    }

    if (x_overlap < y_overlap) {
        manifold->x = x_overlap;
        if (pos1->x > pos2->x) {
            manifold->x *= -1;
        }
    } else {
        manifold->y = y_overlap;
        if (pos1->y > pos2->y) {
            manifold->y *= -1;
        }
    }

    return true;
}

static int MK_Game_ResolveCollisions(MK_Game *game, float dt) {
    assert_return_zero(!game->debug_no_collision_resolution);

    for (MK_EntityID e_id_a = 0; e_id_a < MK_ENTITYID_MAX; e_id_a++) {
        MK_Vec2 *e_pos1 = &game->universe.e_position[e_id_a];
        MK_Vec2 *e_vel1 = &game->universe.e_velocity[e_id_a];
        MK_Vec2 *e_siz1 = &game->universe.e_size[e_id_a];
        MK_Mass *e_mas1 = &game->universe.e_mass[e_id_a];
        for (MK_EntityID e_id_b = (MK_EntityID)(e_id_a + 1); e_id_b < MK_ENTITYID_MAX; e_id_b++) {
            MK_Vec2 *e_pos2 = &game->universe.e_position[e_id_b];
            MK_Vec2 *e_vel2 = &game->universe.e_velocity[e_id_b];
            MK_Vec2 *e_siz2 = &game->universe.e_size[e_id_b];
            MK_Mass *e_mas2 = &game->universe.e_mass[e_id_b];

            if (!e_mas1->invMass && !e_mas2->invMass) {
                // both are immovable
                continue;
            }

            MK_Manifold manifold = { 0 };

            if (MK_Intersects(&manifold, e_pos1, e_siz1, e_pos2, e_siz2)) {
                if (!e_mas2->invMass) {
                    e_pos1->x -= manifold.x;
                    e_pos1->y -= manifold.y;
                    if (manifold.x) e_vel1->x *= -(1.0f - 0.05f * dt);
                    if (manifold.y) e_vel1->y *= -(1.0f - 0.05f * dt);
                } else if (!e_mas1->invMass) {
                    e_pos2->x += manifold.x;
                    e_pos2->y += manifold.y;
                    if (manifold.x) e_vel2->x *= -(1.0f - 0.05f * dt);
                    if (manifold.y) e_vel2->y *= -(1.0f - 0.05f * dt);
                } else {
                    // Position correction
                    float alpha1 = e_mas1->invMass / (e_mas1->invMass + e_mas2->invMass);
                    float alpha2 = 1.0f - alpha1;
                    e_pos1->x -= manifold.x * alpha1;
                    e_pos1->y -= manifold.y * alpha1;
                    e_pos2->x += manifold.x * alpha2;
                    e_pos2->y += manifold.y * alpha2;

                    // Collision response
                    float m1 = 1.0f / e_mas1->invMass;
                    float m2 = 1.0f / e_mas2->invMass;

                    float v1_coef_v1 = (m1 - m2) / (m1 + m2);
                    float v1_coef_v2 = (m2 + m2) / (m1 + m2);
                    float v2_coef_v1 = (m1 + m1) / (m1 + m2);
                    float v2_coef_v2 = (m1 - m2) / (m1 + m2);

                    float v1_x = v1_coef_v1 * e_vel1->x + v1_coef_v2 * e_vel2->x;
                    float v1_y = v1_coef_v1 * e_vel1->y + v1_coef_v2 * e_vel2->y;
                    float v2_x = v2_coef_v1 * e_vel1->x - v2_coef_v2 * e_vel2->x;
                    float v2_y = v2_coef_v1 * e_vel1->y - v2_coef_v2 * e_vel2->y;

                    if (manifold.x) {
                        e_vel1->x = v1_x * (1.0f - 0.95f * dt);
                        e_vel2->x = v2_x * (1.0f - 0.95f * dt);
                    }
                    if (manifold.y) {
                        e_vel1->y = v1_y * (1.0f - 0.95f * dt);
                        e_vel2->y = v2_y * (1.0f - 0.95f * dt);
                    }
                }
            }
        }
    }
    return MK_SUCCESS;
}

int MK_Game_Simulate(MK_Game *game, float dt) {
    float iterDt = dt / MK_GAME_PHYSICS_ITERATIONS;
    for (int iter = 0; iter < MK_GAME_PHYSICS_ITERATIONS; iter++) {
        succeed_or_return_expr(MK_Game_UpdateSpatialGrid(game));
        succeed_or_return_expr(MK_Game_SimulatePhysics(game, iterDt));
        succeed_or_return_expr(MK_Game_ResolveCollisions(game, iterDt));
    }
    return MK_SUCCESS;
}