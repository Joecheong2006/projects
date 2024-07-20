#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>
#include "camera.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"
#include <string.h>
#include "chess_board.h"
#include "anim_duration.h"
#include "anim_position_slide.h"
#include "anim_duration_system.h"

#include "memallocate.h"

#define WIDTH 640
#define HEIGHT 640

#define PI 3.14159265359

typedef struct {
    camera cam;
    chess_board board;
    chess* hold;
    vec2 cursor_index;
} Game;

void chess_move_anim_callback(anim_position_slide* slide, float dur) {
    const float t = sinf(dur * PI * 0.5);
    float* local_position = (*slide->target);
    glm_vec2_lerp(slide->start, slide->end, t, local_position);
}

static vec2 window_size;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Game* game = glfwGetWindowUserPointer(window);
    glViewport(0, 0, width, height);
    set_camera_ortho_mat4(game->cam.ortho, (vec2){width, height});

    window_size[0] = width;
    window_size[1] = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        static int line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, (line_mode = !line_mode) ? GL_LINE : GL_FILL);
    }
    Game* game = glfwGetWindowUserPointer(window);
    float step = 1.0 / 144 * 10;
    vec3 direction;
    direction[2] = 0;
    if (key == GLFW_KEY_UP) {
        translate_camera(&game->cam, (vec3){0, step, 0});
    }
    else if (key == GLFW_KEY_DOWN) {
        translate_camera(&game->cam, (vec3){0, -step, 0});
    }
    if (key == GLFW_KEY_LEFT) {
        translate_camera(&game->cam, (vec3){-step, 0, 0});
    }
    else if (key == GLFW_KEY_RIGHT) {
        translate_camera(&game->cam, (vec3){step, 0, 0});
    }

    if (key == GLFW_KEY_W) {
        game->board.tran.position[1] += step;
    }
    else if (key == GLFW_KEY_S) {
        game->board.tran.position[1] -= step;
    }
    if (key == GLFW_KEY_A) {
        game->board.tran.position[0] -= step;
    }
    else if (key == GLFW_KEY_D) {
        game->board.tran.position[0] += step;
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    Game* game = glfwGetWindowUserPointer(window);
    vec4 uv = { xpos / window_size[0] * 2 - 1, (1 - ypos / window_size[1]) * 2 - 1, 0, 1};
    mat4 m;
    glm_mat4_mul(game->cam.ortho, game->cam.view, m);
    glm_mat4_inv(m, m);
    glm_mat4_mulv(m, uv, uv);

    glm_vec2_copy(game->board.tran.position, game->cursor_index);
    game->cursor_index[0] = (int)(uv[0] - game->cursor_index[0] + 4);
    game->cursor_index[1] = (int)(uv[1] - game->cursor_index[1] + 4);
    glm_vec2_clamp(game->cursor_index, 0, 7);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Game* game = glfwGetWindowUserPointer(window);

    static vec2 pre_index;
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        if (game->hold) {
            chess* target = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (target == game->hold) {
                return;
            }

            if ((game->board.around % 2 == 0 && game->hold->is_white) || (game->board.around % 2 && !game->hold->is_white)) {
                game->hold = NULL;
                return;
            }

            int legal = game->hold->is_legal_move(&game->board, pre_index, game->cursor_index);
            if (!legal) {
                game->hold = NULL;
                return;
            }

            printf("%s\n", game->board.around % 2 == 0 ? "white" : "black");

            game->board.around++;
            chess target_copy, hold_copy;
            chess_copy(target, &target_copy);
            chess_copy(game->hold, &hold_copy);

            game->hold->first_move = 0;
            chess_copy(game->hold, target);
            game->hold->type = ChessTypeDead;

            if (king_is_checked(&game->board)) {
                chess_copy(&target_copy, target);
                chess_copy(&hold_copy, game->hold);
                game->hold = NULL;
                game->board.around--;
                return;
            }

            vec3 offset = {0, 0, 0};
            glm_vec2_sub(game->cursor_index, pre_index, offset);
            init_anim_position_slide(&target->anim, offset, chess_move_anim_callback);
            anim_duration anim;
            set_anim_position_slide(&target->anim, &target->anim, &target->tran.local_position);
            init_anim_position_slide_duration(&anim, &target->anim, 0.12);
            create_anim_duration(&anim);
            game->hold = NULL;
        }
        else {
            game->hold = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (game->hold->type == ChessTypeDead) {
                game->hold = NULL;
            }
            glm_vec2_copy(game->cursor_index, pre_index);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_2) {
        game->hold = NULL;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);
    glfwSwapInterval(1);

    stbi_set_flip_vertically_on_load(1);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    GLC(glEnable(GL_DEPTH_TEST));
    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLC(glDepthFunc(GL_LESS));
    // GLC(glAlphaFunc(GL_GREATER, 0.1));
    // GLC(glEnable(GL_ALPHA_TEST));
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // setting up
    setup_anim_system();
    init_sprite_instance();

    sprite_texture chess_pieces_texture = { .per_sprite = {1.0 / 12, 1} };
    init_texture(&chess_pieces_texture.tex, "chess/classic/spritesheet.png");

    sprite_texture chess_board_texture = { .per_sprite = {1, 1} };
    init_texture(&chess_board_texture.tex, "chess/classic/board.png");

    Game game;
    glfwSetWindowUserPointer(window, &game);

    init_camera(&game.cam, (vec2){WIDTH, HEIGHT});
    init_chess_board(&game.board);
    game.hold = NULL;
    
    game.board.tran.position[0] = 0;
    game.board.tran.position[1] = 0;
    printf("white\n");

    glClearColor(0.0, 0.1, 0.1, 0);

    anim_position_slide chess_move_anim;
    init_anim_position_slide(&chess_move_anim, (vec3){0, 1, 0}, chess_move_anim_callback);

    anim_position_slide anim_slots[8];

    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_anim_system();

		render_chess_board(&game.cam, &game.board, &chess_board_texture, &chess_pieces_texture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_anim_system();

    glfwDestroyWindow(window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
