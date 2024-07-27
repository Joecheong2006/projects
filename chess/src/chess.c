#include "chess.h"
#include "chess_board.h"
#include "anim_duration_system.h"

static vec2 chess_pieces_sprite_indecs[] = {
    [ChessTypeKing] = {1, 0},
    [ChessTypePawn] = {3, 0},
    [ChessTypeQueen] = {4, 0},
    [ChessTypeKnight] = {2, 0},
    [ChessTypeBishop] = {0, 0},
    [ChessTypeRook] = {5, 0},
};


static void default_recovery_callback(chess_board* board, vec2 start) {
	(void)board, (void)start;
}

static void left_en_passant_recovery_callback(chess_board* board, vec2 start) {
	chess* che = get_chess_from_board(board, start[0] - 1, start[1]);
	che->type = ChessTypePawn;
}

static void right_en_passant_recovery_callback(chess_board* board, vec2 start) {
	chess* che = get_chess_from_board(board, start[0] + 1, start[1]);
	che->type = ChessTypePawn;
}

static int check_move_callback(chess_board* board, vec2 start, vec2 end) {
	(void)board, (void)start, (void)end;
	return 0;
}

static void default_move_callback(chess_board* board, vec2 start, vec2 end) {
	(void)board, (void)start, (void)end;
}

void chess_move_anim_callback(anim_position_slide* slide, float dur) {
    const float t = sinf(dur * 3.14152 * 0.5);
    float* local_position = *slide->target;
    glm_vec2_lerp(slide->start, slide->end, t, local_position);
}

static void short_castle_move_callback(chess_board* board, vec2 start, vec2 end) {
	chess* rook = get_chess_from_board(board, end[0] + 1, end[1]);
	chess* che = get_chess_from_board(board, start[0] + 1, start[1]);
	*che = *rook;
	rook->type = ChessTypeDead;
	che->type = ChessTypeRook;
    init_anim_position_slide(&che->anim, (vec3){-2, 0, 0}, chess_move_anim_callback);
    anim_duration anim;
    set_anim_position_slide(&che->anim, &che->tran.local_position);
    init_anim_position_slide_duration(&anim, &che->anim, 0.12);
    create_anim_duration(&anim);
}

static void long_castle_move_callback(chess_board* board, vec2 start, vec2 end) {
	chess* rook = get_chess_from_board(board, end[0] - 2, end[1]);
	chess* che = get_chess_from_board(board, start[0] - 1, start[1]);
	*che = *rook;
	rook->type = ChessTypeDead;
	che->type = ChessTypeRook;
    init_anim_position_slide(&che->anim, (vec3){3, 0, 0}, chess_move_anim_callback);
    anim_duration anim;
    set_anim_position_slide(&che->anim, &che->tran.local_position);
    init_anim_position_slide_duration(&anim, &che->anim, 0.12);
    create_anim_duration(&anim);
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
	current->recover_illegal_move = default_recovery_callback;

	if (offset[0] == 0 && offset[1] == direction) {
		return !chess_exist(board, start[0], start[1] + direction);
	}

	if (offset[0] == 0 && first_move == 2 && offset[1] == 2 * direction) {
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
		if (current->is_white != left->is_white && left->en_passant && left->first_move == 1 && left->type != ChessTypeDead) {
			left->type = ChessTypeDead;
			current->recover_illegal_move = left_en_passant_recovery_callback;
			return 1;
		}
	}

	if (offset[0] == 1 && offset[1] == direction) {
		chess* top_right = get_chess_from_board(board, start[0] + 1, start[1] + direction);
		if (current->is_white != top_right->is_white && top_right->type != ChessTypeDead) {
			return 1;
		}
		chess* right = get_chess_from_board(board, start[0] + 1, start[1]);
		if (current->is_white != right->is_white && right->en_passant && right->first_move == 1 && right->type != ChessTypeDead) {
			right->type = ChessTypeDead;
			current->recover_illegal_move = right_en_passant_recovery_callback;
			return 1;
		}
	}

	return 0;
}

static int check_if_king_in_position(chess_board* board, int x, int y) {
	chess* left = get_chess_from_board(board, x, y);
	left->type = ChessTypeKing;
	board->round++;
	int is_checked = king_is_checked(board);
	board->round--;
	left->type = ChessTypeDead;
	return is_checked;
}

static int king_move_callback(chess_board* board, vec2 start, vec2 end) {
	vec2 offset;
	glm_vec2_sub(end, start, offset);
	chess* current = get_chess_from_board(board, start[0], start[1]);
	if (offset[0] * offset[0] > 1 || offset[1] * offset[1] > 1) {
		if (offset[0] == -2) {
			chess* che = get_chess_from_board(board, start[0] - 4, start[1]);
			if (!chess_exist(board, start[0] - 1, start[1]) &&
			    !chess_exist(board, start[0] - 2, start[1]) &&
			    !chess_exist(board, start[0] - 3, start[1]) &&
			    che->first_move == 2 && che->type == ChessTypeRook &&
			    current->first_move == 2) {

				if (!(check_if_king_in_position(board, start[0] - 1, start[1]) ||
					check_if_king_in_position(board, start[0] - 2, start[1])))
				{
					current->move = long_castle_move_callback;
					return 1;
				}
				return 0;
			}
		}
		else if (offset[0] == 2) {
			chess* che = get_chess_from_board(board, start[0] + 3, start[1]);
			if (!chess_exist(board, start[0] + 1, start[1]) &&
			    !chess_exist(board, start[0] + 2, start[1]) &&
			    che->first_move == 2 && che->type == ChessTypeRook &&
			    current->first_move == 2) {
				if (!(check_if_king_in_position(board, start[0] + 1, start[1]) ||
					check_if_king_in_position(board, start[0] + 2, start[1])))
				{
					current->move = short_castle_move_callback;
					return 1;
				}
				return 0;
			}
		}
		return 0;
	}

	chess* che = get_chess_from_board(board, end[0], end[1]);
	if (che->type != ChessTypeDead && che->is_white == current->is_white) {
		return 0;
	}
	current->move = default_move_callback;
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
    [ChessTypeDead] = check_move_callback,
};

void init_chess(chess_board* board, chess* che, ChessType type, i32 is_white, vec2 position) {
    init_transform(&che->tran);
	glm_vec3_copy((vec3){position[0] - 3.5, position[1] - 3.5, 0.4}, che->tran.local_position);
    che->tran.parent = &board->tran;

    che->en_passant = 0;
    che->type = type;
    che->is_legal_move = map_legal_move_callback[che->type];
    che->first_move = 2;
    che->is_white = is_white;
    che->recover_illegal_move = default_recovery_callback;
    che->move = default_move_callback;
    glm_vec2_copy(chess_pieces_sprite_indecs[(int)type], che->sp.sprite_index);
    che->background = (sprite) {
    	.sprite_index = {12, 0},
    	.color = {94.0 / 255, 215.0 / 255, 241.0 / 255, 0.0}
    };
    if (is_white) {
        che->sp.sprite_index[0] += 6;
    }
    glm_vec4_copy((vec4){1, 1, 1, 1}, che->sp.color);
}

void render_chess_piece(camera* cam, chess* che, sprite_texture* chess_tex) {
	if (che->type != ChessTypeDead) {
	    render_sprite(cam, &che->tran, chess_tex, &che->sp);
	}
}

