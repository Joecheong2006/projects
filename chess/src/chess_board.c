#include "chess_board.h"

chess* get_chess_from_board(chess_board* board, int x, int y) {
	int index = y * 8 + x;
	return &board->grid[index];
}

void init_chess_board(chess_board* board) {
	init_transform(&board->tran);
	glm_vec3_copy((vec3){8, 8, 8}, board->tran.scale);

	board->round = 1;

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

typedef int(*check_king_callback)(chess_board* board, vec2 cur);

static int is_king_checked(chess_board* board, int is_white, int x, int y) {
	chess* che = get_chess_from_board(board, x, y);
	if (x > 7 || x < 0 || y > 7 || y < 0) {
		return 0;
	}
	return che->type == ChessTypeKing && is_white != che->is_white;
}

static int pawn_check_callback(chess_board* board, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	int direction = current->is_white ? 1 : -1;
	return is_king_checked(board, current->is_white, cur[0] - 1, cur[1] + direction) ||
		   is_king_checked(board, current->is_white, cur[0] + 1, cur[1] + direction);
}

static int knight_check_callback(chess_board* board, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	return is_king_checked(board, current->is_white, cur[0] + 1, cur[1] + 2) ||
		   is_king_checked(board, current->is_white, cur[0] - 1, cur[1] + 2) ||
		   is_king_checked(board, current->is_white, cur[0] - 1, cur[1] - 2) ||
		   is_king_checked(board, current->is_white, cur[0] + 1, cur[1] - 2) ||
		   is_king_checked(board, current->is_white, cur[0] + 2, cur[1] + 1) ||
		   is_king_checked(board, current->is_white, cur[0] - 2, cur[1] + 1) ||
		   is_king_checked(board, current->is_white, cur[0] - 2, cur[1] - 1) ||
		   is_king_checked(board, current->is_white, cur[0] + 2, cur[1] - 1);
}

static int check_kind_with_direction(chess_board* board, vec2 diagonal, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	for (int i = 1; i < 8; ++i) {
		int x = cur[0] + i * diagonal[0], y = cur[1] + i * diagonal[1];
		if (x == 8 || x == -1 || y == 8 || y == -1) {
			break;
		}
		chess* che = get_chess_from_board(board, x, y);
		if (che->type == ChessTypeKing && current->is_white != che->is_white) {
			return 1;
		}
		if (che->type != ChessTypeDead) {
			 return 0;
		}
	}
	return 0;
}

static int bishop_check_callback(chess_board* board, vec2 cur) {
	return check_kind_with_direction(board, (vec2){1, 1}, cur) ||
		   check_kind_with_direction(board, (vec2){-1, 1}, cur) ||
		   check_kind_with_direction(board, (vec2){1, -1}, cur) ||
		   check_kind_with_direction(board, (vec2){-1, -1}, cur);
}

static int rook_check_callback(chess_board* board, vec2 cur) {
	return check_kind_with_direction(board, (vec2){1, 0}, cur) ||
		   check_kind_with_direction(board, (vec2){0, 1}, cur) ||
		   check_kind_with_direction(board, (vec2){-1, 0}, cur) ||
		   check_kind_with_direction(board, (vec2){0, -1}, cur);
}

static int queen_check_callback(chess_board* board, vec2 cur) {
	return rook_check_callback(board, cur) || bishop_check_callback(board, cur);
}

static int king_check_callback(chess_board* board, vec2 cur) {
	return 0;
}

static check_king_callback map_king_check_callback[] = {
    [ChessTypeKing] = king_check_callback,
    [ChessTypePawn] = pawn_check_callback,
    [ChessTypeQueen] = queen_check_callback,
    [ChessTypeKnight] = knight_check_callback,
    [ChessTypeBishop] = bishop_check_callback,
    [ChessTypeRook] = rook_check_callback,
    [ChessTypeDead] = NULL,
};

int king_is_checked(chess_board* board) {
	int is_white = board->round % 2;
	for (i32 i = 0; i < 64; i++) {
	    if (board->grid[i].type != ChessTypeDead && board->grid[i].is_white == is_white) {
	    	if (map_king_check_callback[board->grid[i].type](board, (vec2){i % 8, (int)(i / 8)})) {
	    		printf("%d checked %s king\n", board->grid[i].type, is_white ? "white" : "black");
	    		return 1;
	    	}
	    }
	}
	return 0;
}

void board_reset_en_passant(chess_board* board) {
	for (i32 i = 0; i < 64; i++) {
	    if (board->grid[i].type == ChessTypePawn && board->grid[i].is_white == board->round % 2) {
	        board->grid[i].en_passant = 0;
	    }
	}
}
