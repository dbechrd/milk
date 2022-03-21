#include "mk_game.h"

#define game_guard() \
    assert_return_guard(game); \
    assert_return_guard(player_slot >= 0); \
    assert_return_guard(player_slot < MK_GAME_PLAYERS_MAX);

int MK_Game_Init(MK_Game *game) {
    succeed_or_return_expr(MK_Game_PlayerInit(game, 0, { 255, 0, 0, 255 }, { 100, 100 }));
    succeed_or_return_expr(MK_Game_PlayerInit(game, 1, { 0, 255, 0, 255 }, { 500, 100 }));
    return MK_SUCCESS;
}

int MK_Game_PlayerInit(MK_Game *game, int player_slot, SDL_Color color, SDL_Point position) {
    game_guard();

    succeed_or_return_expr(MK_Universe_Create(&game->universe, &game->player_ids[player_slot]));
    succeed_or_return_expr(MK_Game_PlayerSetColor(game, player_slot, color));
    succeed_or_return_expr(MK_Game_PlayerSetPos(game, player_slot, position));
    return MK_SUCCESS;
}

int MK_Game_PlayerGetColor(MK_Game *game, int player_slot, SDL_Color *color) {
    game_guard();
    assert_return_guard(color);

    MK_Attr_Color col{};
    succeed_or_return_expr(MK_Universe_GetColor(&game->universe, game->player_ids[player_slot], &col));
    *color = col.color;
    return MK_SUCCESS;
}

int MK_Game_PlayerSetColor(MK_Game *game, int player_slot, const SDL_Color color) {
    game_guard();

    MK_Attr_Color col{};
    col.color = color;
    succeed_or_return_expr(MK_Universe_SetColor(&game->universe, game->player_ids[player_slot], &col));
    return MK_SUCCESS;
}

int MK_Game_PlayerGetPos(MK_Game *game, int player_slot, SDL_Point *position) {
    game_guard();
    assert_return_guard(position);

    MK_Attr_Position pos{};
    succeed_or_return_expr(MK_Universe_GetPos(&game->universe, game->player_ids[player_slot], &pos));
    *position = pos.position;
    return MK_SUCCESS;
}

int MK_Game_PlayerSetPos(MK_Game *game, int player_slot, const SDL_Point position) {
    game_guard();

    MK_Attr_Position pos{};
    pos.position = position;
    succeed_or_return_expr(MK_Universe_SetPos(&game->universe, game->player_ids[player_slot], &pos));
    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, int player_slot, int x, int y) {
    game_guard();

    MK_Attr_Position pos{};
    succeed_or_return_expr(MK_Universe_GetPos(&game->universe, game->player_ids[player_slot], &pos));
    pos.position.x += x;
    pos.position.y += y;
    succeed_or_return_expr(MK_Universe_SetPos(&game->universe, game->player_ids[player_slot], &pos));
    return MK_SUCCESS;
}