#ifndef _CHESS_BOARD_
#define _CHESS_BOARD_
#include <cglm/cglm.h>
#include "sprite.h"

typedef enum {
    ChessTypeKing,
    ChessTypePawn,
    ChessTypeQueen,
    ChessTypeKnight,
    ChessTypeBishop,
    ChessTypeRook,
    ChessTypeDead,
} ChessType;

typedef struct chess_board chess_board;
typedef struct {
    sprite sp;
    transform tran;
    ChessType type;
    i32 en_passant;
} chess;

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position);

struct chess_board {
    chess grid[64];
    transform tran;
    sprite sp;
};

void init_chess_board(chess_board* board);
void render_chess_board(camera* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex);

#endif
