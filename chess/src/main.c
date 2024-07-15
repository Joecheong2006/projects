#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera2d.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"

#include "memallocate.h"

#define WIDTH 640
#define HEIGHT 640

typedef struct {
    char* title;
} window_data;

void test_controller() {
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (1 == present) {
        int count;
        const float* axes = glfwGetJoystickAxes(present, &count);
        if (axes) {
            printf("%7.3g, %7.3g, %7.3g, %7.3g, %7.3g, %7.3g\n", axes[0], axes[1], axes[2], axes[3], axes[4], axes[5]);
        }
        const unsigned char* buttons = glfwGetJoystickButtons(present, &count);
        if (buttons) {
            for (int i = 0; i < 10; i++) {
                if (buttons[i] == GLFW_PRESS) {
                    printf("b[%d] ", i);
                }
            }
            printf("\n");
        }
        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        printf("%s\n", name);
	}
}

typedef enum {
    ChessTypeKing,
    ChessTypePawn,
    ChessTypeQueen,
    ChessTypeKnight,
    ChessTypeBishop,
    ChessTypeRook,
    ChessTypeDead,
} ChessType;

vec2 chess_pieces_sprite_indecs[] = {
    [ChessTypeKing] = {1, 0},
    [ChessTypePawn] = {3, 0},
    [ChessTypeQueen] = {4, 0},
    [ChessTypeKnight] = {2, 0},
    [ChessTypeBishop] = {0, 0},
    [ChessTypeRook] = {5, 0},
};

typedef struct {
    sprite sp;
    transform2d tran;
    ChessType type;
    i32 en_passant;
} chess;

void init_chess(chess* che, ChessType type, i32 is_white, vec2 position) {
    che->tran = (transform2d){
        .position = {0, 0},
        .scale = {1, 1}
    };
    che->sp.tran = (transform2d){
        .position = {position[0], position[1]},
        .scale = {1, 1}
    };
    che->en_passant = 0;
    che->type = type;
    glm_vec2_copy(chess_pieces_sprite_indecs[(int)type], che->sp.sprite_index);
    if (is_white) {
        che->sp.sprite_index[0] += 6;
    }
}

typedef struct {
    chess cells[64];
    transform2d tran;
    sprite sp;
} chess_board;

void init_chess_board(chess_board* board) {
    board->tran = (transform2d) {
        .position = {0, 0},
        .scale = {1, 1}
    };
	board->sp = (sprite){
	    .tran = {
	        .position = {0, 0},
	        .scale = {8, 8}
		},
	    .sprite_index = {0, 0}
	};

    for (int i = 0; i < 64; i++) {
        board->cells[i].type = ChessTypeDead;
    }
	for (int i = 0; i < 8; i++) {
		init_chess(&board->cells[1 * 8 + i], ChessTypePawn, 1, (vec2){i, 1});
	}
	for (int i = 0; i < 8; i++) {
		init_chess(&board->cells[6 * 8 + i], ChessTypePawn, 0, (vec2){i, 6});
	}
	init_chess(&board->cells[7 * 8 + 0], ChessTypeRook, 0, (vec2){0, 7});
	init_chess(&board->cells[7 * 8 + 7], ChessTypeRook, 0, (vec2){7, 7});
	init_chess(&board->cells[0 * 8 + 0], ChessTypeRook, 1, (vec2){0, 0});
	init_chess(&board->cells[0 * 8 + 7], ChessTypeRook, 1, (vec2){7, 0});

	init_chess(&board->cells[7 * 8 + 1], ChessTypeKnight, 0, (vec2){1, 7});
	init_chess(&board->cells[7 * 8 + 6], ChessTypeKnight, 0, (vec2){6, 7});
	init_chess(&board->cells[0 * 8 + 1], ChessTypeKnight, 1, (vec2){1, 0});
	init_chess(&board->cells[0 * 8 + 6], ChessTypeKnight, 1, (vec2){6, 0});

	init_chess(&board->cells[7 * 8 + 2], ChessTypeBishop, 0, (vec2){2, 7});
	init_chess(&board->cells[7 * 8 + 5], ChessTypeBishop, 0, (vec2){5, 7});
	init_chess(&board->cells[0 * 8 + 2], ChessTypeBishop, 1, (vec2){2, 0});
	init_chess(&board->cells[0 * 8 + 5], ChessTypeBishop, 1, (vec2){5, 0});

	init_chess(&board->cells[7 * 8 + 3], ChessTypeQueen, 0, (vec2){3, 7});
	init_chess(&board->cells[7 * 8 + 4], ChessTypeKing, 0, (vec2){4, 7});
	init_chess(&board->cells[0 * 8 + 3], ChessTypeQueen, 1, (vec2){3, 0});
	init_chess(&board->cells[0 * 8 + 4], ChessTypeKing, 1, (vec2){4, 0});
}

void render_chess_board(camera2d* cam, chess_board* board, sprite_texture* board_tex, sprite_texture* chess_tex) {
    board->sp.tran.position[0] += board->tran.position[0];
    board->sp.tran.position[1] += board->tran.position[1];
	render_sprite(cam, board_tex, &board->sp);
    board->sp.tran.position[0] -= board->tran.position[0];
    board->sp.tran.position[1] -= board->tran.position[1];

	for (i32 i = 0; i < 64; i++) {
	    if (board->cells[i].type != ChessTypeDead) {
	        board->cells[i].sp.tran.position[0] += board->tran.position[0];
	        board->cells[i].sp.tran.position[1] += board->tran.position[1];
    	    render_sprite(cam, chess_tex, &board->cells[i].sp);
	        board->cells[i].sp.tran.position[0] -= board->tran.position[0];
	        board->cells[i].sp.tran.position[1] -= board->tran.position[1];
	    }
	}
}

typedef struct {
    camera2d cam;
    chess_board board;
} Game;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Game* game = glfwGetWindowUserPointer(window);
    glViewport(0, 0, width, height);
    set_camera_ortho_mat4(game->cam.ortho, (vec2){game->cam.canvas[0] * (float)width / height, game->cam.canvas[1]});
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        static int line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, (line_mode = !line_mode) ? GL_LINE : GL_FILL);
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}


int main(void)
{
    stbi_set_flip_vertically_on_load(1);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // setting up chess
    init_sprite_instance();

    sprite_texture chess_pieces_texture = { .per_sprite = {1.0 / 12, 1} };
    init_texture(&chess_pieces_texture.tex, "chess/classic/spritesheet.png");

    sprite_texture chess_board_texture = { .per_sprite = {1, 1} };
    init_texture(&chess_board_texture.tex, "chess/classic/board.png");

    Game game;
    glfwSetWindowUserPointer(window, &game);

    init_camera2d(&game.cam, (vec2){5, 5});
    // translate_camera2d(&cam, (vec2){-1, 1});

    init_chess_board(&game.board);
    
    game.board.tran.position[0] = -3.5;
    game.board.tran.position[1] = -3.5;

    game.board.sp.tran.position[0] = 3.5;
    game.board.sp.tran.position[1] = 3.5;

    glClearColor(0.1, 0.1, 0.1, 0);

    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT);
		
		render_chess_board(&game.cam, &game.board, &chess_board_texture, &chess_pieces_texture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    glfwDestroyWindow(window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
