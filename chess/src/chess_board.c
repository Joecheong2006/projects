#include "chess_board.h"

static vec2 chess_pieces_sprite_indecs[] = {
    [ChessTypeKing] = {1, 0},
    [ChessTypePawn] = {3, 0},
    [ChessTypeQueen] = {4, 0},
    [ChessTypeKnight] = {2, 0},
    [ChessTypeBishop] = {0, 0},
    [ChessTypeRook] = {5, 0},
};

static int default_move_callback(chess_board* board, vec2 start, vec2 end) {
	return 1;
}

chess* get_chess_from_board(chess_board* board, int x, int y) {
	int index = y * 8 + x;
	return &board->grid[index];
}

static i32 chess_exist(chess_board* board, int x, int y) {
	chess* result = get_chess_from_board(board, x, y);
	return result && result->type != ChessTypeDead;
}

static int pawn_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);

	chess* current = get_chess_from_board(board, start[0], start[1]);
	int first_move = current->first_move;
	int direction = current->is_white ? 1 : -1;

	if (offset[0] == 0 && offset[1] == direction) {
		return !chess_exist(board, start[0], start[1] + direction);
	}

	if (offset[0] == 0 && first_move && offset[1] == 2 * direction) {
		chess* left = get_chess_from_board(board, start[0] - 1, start[1] + 2 * direction);
		chess* right = get_chess_from_board(board, start[0] + 1, start[1] + 2 * direction);
		if (left->type == ChessTypePawn && left->is_white != current->is_white) {
			current->en_passant = 1;
		}
		if (right->type == ChessTypePawn && right->is_white != current->is_white) {
			current->en_passant = 1;
		}
		return !chess_exist(board, start[0], start[1] + direction) && !chess_exist(board, start[0], start[1] + 2 * direction);
	}

	if (offset[0] == -1 && offset[1] == direction) {
		chess* top_left = get_chess_from_board(board, start[0] - 1, start[1] + direction);
		if (current->is_white != top_left->is_white && top_left->type != ChessTypeDead) {
			return 1;
		}
		chess* left = get_chess_from_board(board, start[0] - 1, start[1]);
		if (current->is_white != left->is_white && left->en_passant && left->type != ChessTypeDead) {
			left->type = ChessTypeDead;
			return 1;
		}
	}

	if (offset[0] == 1 && offset[1] == direction) {
		chess* top_right = get_chess_from_board(board, start[0] + 1, start[1] + direction);
		if (current->is_white != top_right->is_white && top_right->type != ChessTypeDead) {
			return 1;
		}
		chess* right = get_chess_from_board(board, start[0] + 1, start[1]);
		if (current->is_white != right->is_white && right->en_passant && right->type != ChessTypeDead) {
			right->type = ChessTypeDead;
			return 1;
		}
	}

	return 0;
}

static int king_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);
	if (offset[0] * offset[0] > 1 || offset[1] * offset[1] > 1) {
		return 0;
	}

	chess* che = get_chess_from_board(board, end[0], end[1]);
	chess* current = get_chess_from_board(board, start[0], start[1]);
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	return 1;
}

static int knight_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);
	int xx = offset[0] * offset[0];
	int yy = offset[1] * offset[1];
	if ((yy != 4 || xx != 1) && (yy != 1 || xx != 4)) {
		return 0;
	}
	chess* che = get_chess_from_board(board, end[0], end[1]);
	chess* current = get_chess_from_board(board, start[0], start[1]);
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	return 1;
}

static int bishop_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);

	if (offset[0] == 0 || offset[1] == 0) {
		return 0;
	}

	vec2 quadrant;
	glm_vec2_copy(offset, quadrant);
	glm_vec2_clamp(quadrant, -1, 1);

	if (offset[0] * offset[0] != offset[1] * offset[1]) {
		return 0;
	}

	int len = offset[0];
	len *= len < 0 ? -1 : 1;

	for (int i = 1; i < len; ++i) {
		if (chess_exist(board, start[0] + i * quadrant[0], start[1] + i * quadrant[1])) {
			return 0;
		}
	}

	chess* current = get_chess_from_board(board, start[0], start[1]);
	chess* che = get_chess_from_board(board, end[0], end[1]);
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	return 1;
}

static int rook_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);

	if ((offset[0] == 0 && offset[1] == 0) || (offset[0] != 0 && offset[1] != 0)) {
		return 0;
	}
	vec2 direction;
	glm_vec2_copy(offset, direction);
	glm_vec2_clamp(direction, -1, 1);

	int len = direction[0] != 0 ? offset[0] : offset[1];
	len *= len < 0 ? -1 : 1;

	for (int i = 1; i < len; ++i) {
		if (chess_exist(board, start[0] + i * direction[0], start[1] + i * direction[1])) {
			return 0;
		}
	}

	chess* current = get_chess_from_board(board, start[0], start[1]);
	chess* che = get_chess_from_board(board, end[0], end[1]);
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	return 1;
}

static int queen_move_callback(chess_board* board, vec2 start, vec2 end) {
	return rook_move_callback(board, start, end) || bishop_move_callback(board, start, end);
}

static check_legal_move_callback map_legal_move_callback[] = {
    [ChessTypeKing] = king_move_callback,
    [ChessTypePawn] = pawn_move_callback,
    [ChessTypeQueen] = queen_move_callback,
    [ChessTypeKnight] = knight_move_callback,
    [ChessTypeBishop] = bishop_move_callback,
    [ChessTypeRook] = rook_move_callback,
    [ChessTypeDead] = default_move_callback,
};

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position) {
    init_transform(&che->tran);
	glm_vec3_copy((vec3){position[0] - 3.5, position[1] - 3.5, 0.2}, che->tran.local_position);
    che->tran.parent = &board->tran;

    che->en_passant = 0;
    che->type = type;
    che->is_legal_move = map_legal_move_callback[che->type];
    che->first_move = 1;
    che->is_white = is_white;
    glm_vec2_copy(chess_pieces_sprite_indecs[(int)type], che->sp.sprite_index);
    if (is_white) {
        che->sp.sprite_index[0] += 6;
    }
}

void chess_copy(chess* src, chess* dest) {
    *dest = *src;
    glm_vec2_copy(src->sp.sprite_index, dest->sp.sprite_index);
    tran_copy(&src->tran, &dest->tran);
}

void init_chess_board(chess_board* board) {
	init_transform(&board->tran);
	glm_vec3_copy((vec3){8, 8, 8}, board->tran.scale);

	board->around = 1;

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
		   is_king_checked(board, current->is_white, cur[0] - 1, cur[1] + direction);
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
	int is_white = board->around % 2;
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
