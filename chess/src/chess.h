#ifndef _CHESS_
#define _CHESS_
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
typedef void(*illegal_move_recovery_callback)(chess_board* board, vec2 start);
typedef void(*legal_move_callback)(chess_board* board, vec2 start, vec2 end);

struct chess {
    sprite sp;
    sprite background;
    transform tran;
    ChessType type;
    anim_position_slide anim;
    check_legal_move_callback is_legal_move;
    illegal_move_recovery_callback recover_illegal_move;
    legal_move_callback move;
    i8 en_passant, first_move, is_white, selected;
};

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position);
void chess_move_anim_callback(anim_position_slide* slide, float dur);

#endif