#include "mk_game.h"

int MK_Game_Init(MK_Game *game) {
    succeed_or_return_expr(MK_Game_PlayerInit(game));
    return MK_SUCCESS;
}

int MK_Game_PlayerInit(MK_Game *game) {
    succeed_or_return_expr(MK_Universe_Create(&game->universe, &game->id_player));
    return MK_SUCCESS;
}

int MK_Game_PlayerGetPos(MK_Game *game, MK_Attr_Position *pos) {
    assert_return_guard(game);
    assert_return_guard(pos);
    assert_return_guard(game->id_player >= 0);
    assert_return_guard(game->id_player < MK_ENTITIES_MAX);
    assert_return_guard(game->universe.exists[game->id_player]);

    *pos = game->universe.position[game->id_player];
    return MK_SUCCESS;
}

int MK_Game_PlayerMove(MK_Game *game, int x, int y) {
    succeed_or_return_expr(MK_Universe_Move(&game->universe, game->id_player, x, y));
    return MK_SUCCESS;
}