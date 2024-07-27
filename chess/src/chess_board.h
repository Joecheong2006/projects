#ifndef _CHESS_BOARD_
#define _CHESS_BOARD_
#include <cglm/cglm.h>
#include "sprite.h"
#include "chess.h"

typedef struct chess_board chess_board;
struct chess_board {
    chess grid[64];
    transform tran;
    sprite sp;
    int round;
};

void init_chess_board(chess_board* board);
chess* get_chess_from_board(chess_board* board, int x, int y);
int king_is_checked(chess_board* board);
int any_possible_move(chess_board* board);
void board_reset_en_passant(chess_board* board);

void render_chess_board(camera* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex);

#endif
