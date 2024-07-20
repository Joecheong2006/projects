#ifndef _CHESS_BOARD_
#define _CHESS_BOARD_
#include <cglm/cglm.h>
#include "anim_position_slide.h"
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

typedef struct chess chess;
typedef struct chess_board chess_board;
typedef int(*check_legal_move_callback)(chess_board* board, vec2 start, vec2 end);

struct chess {
    sprite sp;
    transform tran;
    ChessType type;
    anim_position_slide anim;
    check_legal_move_callback is_legal_move;
    i32 en_passant, first_move, is_white;
};

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position);
void chess_copy(chess* src, chess* dest);

struct chess_board {
    chess grid[64];
    transform tran;
    sprite sp;
};

void init_chess_board(chess_board* board);
void render_chess_board(camera* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex);

#endif
