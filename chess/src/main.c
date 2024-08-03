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

#include "camera_shake.h"

#include "anim_duration.h"
#include "anim_position_slide.h"
#include "anim_duration_system.h"

#include "game_object_system.h"

#include "memallocate.h"

#include "audio.h"

#include "debug/line_renderer.h"

#define PI 3.14159265359

#define WIDTH 640
#define HEIGHT 640

typedef void(*input_control_key_callback)(void* owner, int key, int scancode, int action, int mods);
typedef void(*input_control_cursor_pos_callback)(void* owner, double xpos, double ypos);
typedef void(*input_control_mouse_button_callback)(void* owner, int button, int action, int mods);

typedef struct {
    input_control_key_callback key_callback;
    input_control_cursor_pos_callback cursor_pos_callback;
    input_control_mouse_button_callback mouse_button_callback;
} input_control;

typedef void(*window_control_render_callback)(void* owner);
typedef void(*window_control_resize_callback)(void* window, int width, int height);
typedef struct {
    window_control_render_callback render_callback;
    window_control_resize_callback resize_callback;
} window_control;

typedef struct {
    void* owner;
    input_control input;
    window_control window;
} callback_controller;

typedef struct {
    GLFWwindow* window;
    int width, height;
} window_state;

void close_application(window_state* window) {
    glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

typedef struct {
    chess_board board;
    chess* hold;
    vec2 cursor_index;
    struct {
        sprite_texture board_tex, pieces_tex;
    } chess_theme;
    window_state win_state;
} Game;

void game_window_resize_callback(void* owner, int width, int height) {
    Game* game = owner;
    glViewport(0, 0, width, height);

    camera* cam = find_game_object_by_index(0)->self;
    set_camera_ortho_mat4(cam->projection, (vec2){width, height});

    game->win_state.width = width;
    game->win_state.height = height;
}

void game_key_callback(void* owner, int key, int scancode, int action, int mods) {
    (void)scancode, (void)mods;
    Game* game = owner;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        close_application(&game->win_state);
    }

    if (key == GLFW_KEY_T && action == GLFW_PRESS) {
        static int line_mode = 0;
        glPolygonMode(GL_FRONT_AND_BACK, (line_mode = !line_mode) ? GL_LINE : GL_FILL);
    }
}

void game_cursor_pos_callback(void* owner, double xpos, double ypos) {
    Game* game = owner;
    vec4 uv = { xpos / game->win_state.width * 2 - 1, (1 - ypos / game->win_state.height) * 2 - 1, 0, 1};
    mat4 m;

    camera* cam = find_game_object_by_index(0)->self;
    glm_mat4_mul(cam->projection, cam->view, m);
    glm_mat4_inv(m, m);
    glm_mat4_mulv(m, uv, uv);

    game->cursor_index[0] = (int)(uv[0] - game->board.tran.position[0] + 4);
    game->cursor_index[1] = (int)(uv[1] - game->board.tran.position[1] + 4);
    glm_vec2_clamp(game->cursor_index, 0, 7);
}

void game_mouse_button_callback(void* owner, int button, int action, int mods) {
    (void)mods;
    Game* game = owner;

    static vec2 pre_index;
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        if (game->hold) {
            chess* target = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (target == game->hold) {
                game->hold->background.color[3] = 0;
                game->hold = NULL;
                return;
            }

            if ((game->board.round % 2 == 0 && game->hold->is_white) || (game->board.round % 2 && !game->hold->is_white)) {
                game->hold->background.color[3] = 0;
                game->hold = NULL;
                return;
            }

            int legal = game->hold->is_legal_move(&game->board, pre_index, game->cursor_index);
            if (!legal) {
                game->hold->background.color[3] = 0;
                game->hold = NULL;
                return;
            }

            printf("%s\n", game->board.round % 2 == 0 ? "white" : "black");

            {
                game->board.round++;
                board_reset_en_passant(&game->board);
                chess target_copy = *target, hold_copy = *game->hold;

                game->hold->first_move--;
                *target = *game->hold;
                game->hold->type = ChessTypeDead;

                game->hold->move(&game->board, pre_index, game->cursor_index);

                if (king_is_checked(&game->board)) {
                    *target = target_copy;
                    *game->hold = hold_copy;
                    game->hold->recover_illegal_move(&game->board, pre_index);
                    game->hold->background.color[3] = 0;
                    game->hold = NULL;
                    game->board.round--;
                    return;
                }
            }

        	for (i32 i = 0; i < 64; i++) {
        	    game->board.grid[i].background.color[3] = 0;
        	    game->board.grid[i].selected = 0;
        	}

        	target->selected = 1;
            target->background.color[3] = 0.6;
        	game->hold->selected = 1;
            game->hold->background.color[3] = 0.6;

            if (any_possible_move(&game->board) == 0) {
                game->board.round++;
                int checked = king_is_checked(&game->board);
                game->board.round--;
                if (checked) {
                    printf("mate\n");
                }
                else {
                    printf("sill mate\n");
                }
            }

            static camera_shake_object obj = { .duration = 0.1, .strength = 0.03 };
            create_camera_shake(&obj);

            vec3 offset = {0, 0, 0};
            glm_vec2_sub(game->cursor_index, pre_index, offset);
            init_anim_position_slide(&target->anim, offset, chess_move_anim_callback);
            anim_duration anim;
            set_anim_position_slide(&target->anim, &target->tran.local_position);
            init_anim_position_slide_duration(&anim, &target->anim, 0.12);
            create_anim_duration(&anim);
            game->hold = NULL;
        }
        else {
            game->hold = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (game->hold->selected) {
                game->hold = NULL;
                return;
            }
            if (game->hold->type == ChessTypeDead) {
                game->hold = NULL;
            }
            else {
                game->hold->background.color[3] = 0.6;
            }
            glm_vec2_copy(game->cursor_index, pre_index);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_2) {
        if (game->hold) {
            game->hold->background.color[3] = 0;
        }
        game->hold = NULL;
    }
}

void game_render_callback(void* owner) {
    Game* game = owner;
    camera* cam = find_game_object_by_index(0)->self;
	render_chess_board(cam, &game->board, &game->chess_theme.board_tex, &game->chess_theme.pieces_tex);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->window.resize_callback) {
        c->window.resize_callback(c->owner, width, height);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.key_callback) {
        c->input.key_callback(c->owner, key, scancode, action, mods);
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.cursor_pos_callback) {
        c->input.cursor_pos_callback(c->owner, xpos, ypos);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    callback_controller* c = glfwGetWindowUserPointer(window);
    if (c->input.mouse_button_callback) {
        c->input.mouse_button_callback(c->owner, button, action, mods);
    }
}

void sound_test(const char* path, float pitch, float gain) {
    audio_context audio;
    init_audio(&audio);

    float listenerOri[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, listenerOri);

    u32 source = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);

    u32 buffer = gen_sound_buffer(path);
    if (!buffer) {
        printf("load %s failed\n", path);
        alDeleteSources(1, &source);
        shutdown_audio(&audio);
        return;
    }

    alSourcei(source, AL_BUFFER, buffer);
    al_check_error();

    alSourcePlay(source);
    al_check_error();

    ALint source_state;
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    al_check_error();
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
        al_check_error();
    }

    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    shutdown_audio(&audio);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    stbi_set_flip_vertically_on_load(1);

    GLFWwindow* app_window = glfwCreateWindow(WIDTH, HEIGHT, "chess", NULL, NULL);
    glfwSwapInterval(1);

    glfwSetKeyCallback(app_window, key_callback);
    glfwSetFramebufferSizeCallback(app_window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(app_window, mouse_button_callback);
    glfwSetCursorPosCallback(app_window, cursor_pos_callback);
    
    glfwMakeContextCurrent(app_window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("Opengl Version %s\n", glGetString(GL_VERSION));

    GLC(glEnable(GL_DEPTH_TEST));
    GLC(glEnable(GL_BLEND));
    GLC(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLC(glDepthFunc(GL_LESS));
    // GLC(glAlphaFunc(GL_LESS, 0.1));
    // GLC(glEnable(GL_ALPHA_TEST));
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // glDisable(GL_DITHER);
    // glDisable(GL_POINT_SMOOTH);
    // glDisable(GL_LINE_SMOOTH);
    // glDisable(GL_POLYGON_SMOOTH);
    // glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
    // glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    // glDisable(0x809D);

    audio_context audio;
    init_audio(&audio);

    set_audio_listener_properties((vec3){0, 0, 0}, (vec3){0, 0, 0}, (f32[]){ 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f });

    // setting up
    setup_anim_system();
    init_sprite_instance();
    init_debug_line_renderer_instance();

    Game game;
    game.win_state.window = app_window;
    char* chess_spritesheet = "assets/chess/icy_sea/spritesheet.png";
    char* chess_board = "assets/chess/icy_sea/board.png";

    game.chess_theme.pieces_tex = (sprite_texture){ .per_sprite = {1.0 / 13, 1} };
    init_texture(&game.chess_theme.pieces_tex.tex, chess_spritesheet, TextureFilterLinear);

    game.chess_theme.board_tex = (sprite_texture){ .per_sprite = {1, 1} };
    init_texture(&game.chess_theme.board_tex.tex, chess_board, TextureFilterLinear);

    callback_controller con = (callback_controller){
        .owner = &game,
        .input.cursor_pos_callback = game_cursor_pos_callback,
        .input.key_callback = game_key_callback,
        .input.mouse_button_callback = game_mouse_button_callback,
        .window.resize_callback = game_window_resize_callback,
        .window.render_callback = game_render_callback,
    };

    glfwSetWindowUserPointer(app_window, &con);

    camera cam;
    init_camera(&cam, (vec2){WIDTH, HEIGHT});
    init_chess_board(&game.board);
    game.hold = NULL;
    
    game.board.tran.position[0] = 0;
    game.board.tran.position[1] = 0;

    game.win_state.width = WIDTH;
    game.win_state.height = HEIGHT;
    
    printf("white\n");

    anim_position_slide chess_move_anim;
    init_anim_position_slide(&chess_move_anim, (vec3){0, 1, 0}, chess_move_anim_callback);

    glClearColor(0.0, 0.1, 0.1, 0);

    init_game_object_system();
    create_game_object(&(game_object){
        .self = &cam,
        .on_start = NULL,
        .on_activate = NULL,
        .on_update = NULL,
        .on_render = NULL,
        .on_destory = NULL,
    });

    float pitch = 1, gain = 1;
    u32 buffers[2];
    u32 sources[2];

    {
        sources[0] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        char* audio_file_path = "assets/audio/minecraft1.mp3";
        buffers[0] = gen_sound_buffer(audio_file_path);
        if (!buffers[0]) {
            printf("load %s failed\n", audio_file_path);
            alDeleteSources(1, &sources[0]);
            shutdown_audio(&audio);
            exit(1);
        }

        alSourcei(sources[0], AL_BUFFER, buffers[0]);
        alSourcePlay(sources[0]);
    }
    {
        sources[1] = create_audio_source(pitch, gain, (vec3){0, 0, 0}, (vec3){0, 0, 0}, 0);
        char* audio_file_path = "assets/audio/killshot-speedup.mp3";
        buffers[1] = gen_sound_buffer(audio_file_path);
        if (!buffers[1]) {
            printf("load %s failed\n", audio_file_path);
            alDeleteSources(1, &sources[1]);
            shutdown_audio(&audio);
            exit(1);
        }

        alSourcei(sources[1], AL_BUFFER, buffers[1]);
        // alSourcePlay(sources[1]);
    }

    while(!glfwWindowShouldClose(app_window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float step = 1.0 / 144 * 4;
        if (glfwGetKey(app_window, GLFW_KEY_UP) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){0, step, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){0, -step, 0});
        }
        if (glfwGetKey(app_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){-step, 0, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            translate_camera(&cam, (vec3){step, 0, 0});
        }

        if (glfwGetKey(app_window, GLFW_KEY_W) == GLFW_PRESS) {
            game.board.tran.position[1] += step;
        }
        else if (glfwGetKey(app_window, GLFW_KEY_S) == GLFW_PRESS) {
            game.board.tran.position[1] -= step;
        }
        if (glfwGetKey(app_window, GLFW_KEY_A) == GLFW_PRESS) {
            game.board.tran.position[0] -= step;
        }
        else if (glfwGetKey(app_window, GLFW_KEY_D) == GLFW_PRESS) {
            game.board.tran.position[0] += step;
        }

		update_anim_system();
		con.window.render_callback(con.owner);
        update_game_object_system();

        vec2 offset = { game.board.tran.scale[0] * 0.5,  game.board.tran.scale[1] * 0.5 };
        {
            float points[] = { 0, 0, 1, 0, 0, 1, };
            points[0] = game.board.tran.position[0] + game.board.tran.scale[0] - offset[0];
            points[1] = game.board.tran.position[1] - offset[1];
            points[3] = game.board.tran.position[0] - offset[0];
            points[4] = game.board.tran.position[1] - offset[1];
            render_debug_line(points, (vec3){0, 0, 1});
        }

        glfwSwapBuffers(app_window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_game_object_system();
    shutdown_anim_system();

    alDeleteBuffers(1, &sources[0]);
    alDeleteBuffers(1, &sources[1]);
    alDeleteBuffers(1, &buffers[0]);
    alDeleteBuffers(1, &buffers[1]);
    shutdown_audio(&audio);

    glfwDestroyWindow(app_window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
