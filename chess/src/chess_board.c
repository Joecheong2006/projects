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
	    .sprite_index = {0, 0},
	    .color = {1, 1, 1, 1}
	};

    for (int i = 0; i < 64; i++) {
        board->grid[i].type = ChessTypeDead;
        board->grid[i].selected = 0;
	    board->grid[i].background = (sprite) {
	    	.sprite_index = {12, 0},
	    	.color = {94.0 / 255, 215.0 / 255, 241.0 / 255, 0.0}
	    };
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

sprite_texture background_tex;

void render_chess_board(camera* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex) {
	if (background_tex.per_sprite[0] == 0) {
	    background_tex = (sprite_texture){ .per_sprite = {1, 1} };
	    init_texture(&background_tex.tex, "assets/chess/white_block.png", TextureFilterNearest);
	}
	render_sprite(cam, &board->tran, board_tex, &board->sp);
	for (i32 i = 0; i < 64; i++) {
		transform tran;
		init_transform(&tran);
		tran.parent = &board->tran;
		glm_vec3_copy((vec3){i % 8 - 3.5, i / 8 - 3.5, 0.1}, tran.local_position);
		chess* che = &board->grid[i];
		render_sprite(cam, &tran, &background_tex, &che->background);
		if (board->grid[i].type != ChessTypeDead) {
		    render_sprite(cam, &che->tran, chess_tex, &che->sp);
		}
	}
}

static void find_location(vec2* out, chess_board* board, chess* che) {
	for (int i = 0; i < 64; i++) {
		if (&board->grid[i] == che) {
			glm_vec2_copy((vec2){i % 8, (int)(i / 8)}, *out);
		}
	}
}

typedef int(*check_condition_callback)(chess_board* board, int x, int y, chess* current, int for_check);

int is_legal(chess_board* board, int x, int y, chess* current, int for_check) {
	(void)for_check;
	chess* che = get_chess_from_board(board, x, y);
	{
		vec2 start;
		find_location(&start, board, current);
		if (!current->is_legal_move(board, start, (vec2){x, y})) {
			return 0;
		}
	}
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	chess che_copy = *che;
	chess cur_copy = *current;

	*current = che_copy;
	*che = cur_copy;
	current->type = ChessTypeDead;

	board->round++;
	int checked = king_is_checked(board);
	board->round--;

	*current = cur_copy;
	*che = che_copy;
	return !checked;
}

int is_king(chess_board* board, int x, int y, chess* current, int for_check) {
	(void)current;
	if (!for_check) {
		return 0;
	}
	chess* che = get_chess_from_board(board, x, y);
	return che->type == ChessTypeKing && current->is_white != che->is_white;
}

typedef int(*check_king_callback)(check_condition_callback condition, chess_board* board, vec2 cur);

static int is_king_checked(check_condition_callback condition, chess_board* board, chess* current, int x, int y, int for_check) {
	if (x > 7 || x < 0 || y > 7 || y < 0) {
		return 0;
	}
	return condition(board, x, y, current, for_check);
}

static int pawn_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	int direction = current->is_white ? 1 : -1;
	if (current->first_move == 2 && is_king_checked(condition, board, current, cur[0], cur[1] + direction * 2, 0)) {
		return 1;
	}
	return is_king_checked(condition, board, current, cur[0] - 1, cur[1] + direction, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 1, cur[1] + direction, 1) ||
		   is_king_checked(condition, board, current, cur[0], cur[1] + direction, 0);
}

static int knight_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	return is_king_checked(condition, board, current, cur[0] + 1, cur[1] + 2, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 1, cur[1] + 2, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 1, cur[1] - 2, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 1, cur[1] - 2, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 2, cur[1] + 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 2, cur[1] + 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 2, cur[1] - 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 2, cur[1] - 1, 1);
}

static int check_king_with_direction(check_condition_callback condition, chess_board* board, vec2 diagonal, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	for (int i = 1; i < 8; ++i) {
		int x = cur[0] + i * diagonal[0], y = cur[1] + i * diagonal[1];
		if (x == 8 || x == -1 || y == 8 || y == -1) {
			break;
		}

		chess* che = get_chess_from_board(board, x, y);
		if (condition(board, x, y, current, 1)) {
			return 1;
		}
		if (che->type != ChessTypeDead) {
			 return 0;
		}
	}
	return 0;
}

static int bishop_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	return check_king_with_direction(condition, board, (vec2){1, 1}, cur) ||
		   check_king_with_direction(condition, board, (vec2){-1, 1}, cur) ||
		   check_king_with_direction(condition, board, (vec2){1, -1}, cur) ||
		   check_king_with_direction(condition, board, (vec2){-1, -1}, cur);
}

static int rook_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	return check_king_with_direction(condition, board, (vec2){1, 0}, cur) ||
		   check_king_with_direction(condition, board, (vec2){0, 1}, cur) ||
		   check_king_with_direction(condition, board, (vec2){-1, 0}, cur) ||
		   check_king_with_direction(condition, board, (vec2){0, -1}, cur);
}

static int queen_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	return rook_check_callback(condition, board, cur) || bishop_check_callback(condition, board, cur);
}

static int king_check_callback(check_condition_callback condition, chess_board* board, vec2 cur) {
	chess* current = get_chess_from_board(board, cur[0], cur[1]);
	return is_king_checked(condition, board, current, cur[0] + 1, cur[1] + 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 1, cur[1] - 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 1, cur[1] - 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 1, cur[1] + 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] + 1, cur[1], 1) ||
		   is_king_checked(condition, board, current, cur[0], cur[1] + 1, 1) ||
		   is_king_checked(condition, board, current, cur[0] - 1, cur[1], 1) ||
		   is_king_checked(condition, board, current, cur[0], cur[1] - 1, 1);
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
	    	if (map_king_check_callback[board->grid[i].type](is_king, board, (vec2){i % 8, (int)(i / 8)})) {
	    		return 1;
	    	}
	    }
	}
	return 0;
}

int any_possible_move(chess_board* board) {
	int is_white = board->round % 2;
	for (i32 i = 0; i < 64; i++) {
	    if (board->grid[i].type != ChessTypeDead && board->grid[i].is_white == is_white) {
	    	if (map_king_check_callback[board->grid[i].type](is_legal, board, (vec2){i % 8, (int)(i / 8)})) {
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
