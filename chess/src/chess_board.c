#include "chess_board.h"

static vec2 chess_pieces_sprite_indecs[] = {
    [ChessTypeKing] = {1, 0},
    [ChessTypePawn] = {3, 0},
    [ChessTypeQueen] = {4, 0},
    [ChessTypeKnight] = {2, 0},
    [ChessTypeBishop] = {0, 0},
    [ChessTypeRook] = {5, 0},
};

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position) {
    init_transform(&che->tran);
	// glm_vec3_copy((vec3){position[0] - 3.5, position[1] - 3.5, 1}, che->tran.position);
	glm_vec3_copy((vec3){position[0] - 3.5, position[1] - 3.5, 1}, che->tran.local_position);
    che->tran.parent = &board->tran;

    che->en_passant = 0;
    che->type = type;
    glm_vec2_copy(chess_pieces_sprite_indecs[(int)type], che->sp.sprite_index);
    if (is_white) {
        che->sp.sprite_index[0] += 6;
    }
}

void init_chess_board(chess_board* board) {
	init_transform(&board->tran);
	glm_vec3_copy((vec3){8, 8, 8}, board->tran.scale);

	board->sp = (sprite){
	    .sprite_index = {0, 0}
	};

    for (int i = 0; i < 64; i++) {
        board->grid[i].type = ChessTypeDead;
    }
	for (int i = 0; i < 8; i++) {
		init_chess(board, &board->grid[1 * 8 + i], ChessTypePawn, 1, (vec2){i, 1});
	}
	for (int i = 0; i < 8; i++) {
		init_chess(board, &board->grid[6 * 8 + i], ChessTypePawn, 0, (vec2){i, 6});
	}
	init_chess(board, &board->grid[7 * 8 + 0], ChessTypeRook, 0, (vec2){0, 7});
	init_chess(board, &board->grid[7 * 8 + 7], ChessTypeRook, 0, (vec2){7, 7});
	init_chess(board, &board->grid[0 * 8 + 0], ChessTypeRook, 1, (vec2){0, 0});
	init_chess(board, &board->grid[0 * 8 + 7], ChessTypeRook, 1, (vec2){7, 0});

	init_chess(board, &board->grid[7 * 8 + 1], ChessTypeKnight, 0, (vec2){1, 7});
	init_chess(board, &board->grid[7 * 8 + 6], ChessTypeKnight, 0, (vec2){6, 7});
	init_chess(board, &board->grid[0 * 8 + 1], ChessTypeKnight, 1, (vec2){1, 0});
	init_chess(board, &board->grid[0 * 8 + 6], ChessTypeKnight, 1, (vec2){6, 0});

	init_chess(board, &board->grid[7 * 8 + 2], ChessTypeBishop, 0, (vec2){2, 7});
	init_chess(board, &board->grid[7 * 8 + 5], ChessTypeBishop, 0, (vec2){5, 7});
	init_chess(board, &board->grid[0 * 8 + 2], ChessTypeBishop, 1, (vec2){2, 0});
	init_chess(board, &board->grid[0 * 8 + 5], ChessTypeBishop, 1, (vec2){5, 0});

	init_chess(board, &board->grid[7 * 8 + 3], ChessTypeQueen, 0, (vec2){3, 7});
	init_chess(board, &board->grid[7 * 8 + 4], ChessTypeKing, 0, (vec2){4, 7});
	init_chess(board, &board->grid[0 * 8 + 3], ChessTypeQueen, 1, (vec2){3, 0});
	init_chess(board, &board->grid[0 * 8 + 4], ChessTypeKing, 1, (vec2){4, 0});
}

void render_chess_board(camera* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex) {
	render_sprite(cam, &board->tran, board_tex, &board->sp);
	for (i32 i = 0; i < 64; i++) {
	    if (board->grid[i].type != ChessTypeDead) {
    	    render_sprite(cam, &board->grid[i].tran, chess_tex, &board->grid[i].sp);
	    }
	}
}
