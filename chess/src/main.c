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

#include "game_object_system.h"

#include "memallocate.h"

#include "sound.h"

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
    // camera cam;
    chess_board board;
    chess* hold;
    vec2 cursor_index;
    struct {
        sprite_texture board_tex, pieces_tex;
    } chess_theme;
    window_state win_state;
} Game;

void drop_holding_piece(Game* game) {
    if (game->hold) {
        game->hold->background.color[3] = 0;
    }
    game->hold = NULL;
}

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
            game->hold->background.color[3] = 0;
            chess* target = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (target == game->hold) {
                drop_holding_piece(game);
                return;
            }

            if ((game->board.round % 2 == 0 && game->hold->is_white) || (game->board.round % 2 && !game->hold->is_white)) {
                drop_holding_piece(game);
                return;
            }

            int legal = game->hold->is_legal_move(&game->board, pre_index, game->cursor_index);
            if (!legal) {
                drop_holding_piece(game);
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
                    drop_holding_piece(game);
                    game->board.round--;
                    return;
                }
            }

            vec3 offset = {0, 0, 0};
            glm_vec2_sub(game->cursor_index, pre_index, offset);
            init_anim_position_slide(&target->anim, offset, chess_move_anim_callback);
            anim_duration anim;
            set_anim_position_slide(&target->anim, &target->tran.local_position);
            init_anim_position_slide_duration(&anim, &target->anim, 0.12);
            create_anim_duration(&anim);
            drop_holding_piece(game);
        }
        else {
            game->hold = get_chess_from_board(&game->board, game->cursor_index[0], game->cursor_index[1]);
            if (game->hold->type == ChessTypeDead) {
                drop_holding_piece(game);
            }
            else {
                game->hold->background.color[3] = 0.6;
            }
            glm_vec2_copy(game->cursor_index, pre_index);
        }
    }
    if (button == GLFW_MOUSE_BUTTON_2) {
        drop_holding_piece(game);
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

static void list_audio_devices(const ALCchar *devices)
{
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        fprintf(stdout, "Devices list:\n");
        fprintf(stdout, "----------\n");
        while (device && *device != '\0' && next && *next != '\0') {
                fprintf(stdout, "%s\n", device);
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
        }
        fprintf(stdout, "----------\n");
}

void sound_test(const char* path, float pitch, float gain) {
    ALCdevice* device;
    device = alcOpenDevice(NULL);
    if (!device) {
        printf("failed to open autdio device\n");
    }

    ALboolean enumeration;

    enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE)
        printf("not support enumeration\n");
    else
        printf("support enumeration\n");

    list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    ALCcontext* context;
    context = alcCreateContext(device, NULL);
    if (!alcMakeContextCurrent(context)) {
        printf("failed to make context current\n");
    }
    al_check_error();

    ALfloat listenerOri[] = { 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f };
    
    alListener3f(AL_POSITION, 0, 0, 0.0f);
    al_check_error();
    alListener3f(AL_VELOCITY, 0, 0, 0);
    al_check_error();
    alListenerfv(AL_ORIENTATION, listenerOri);
    al_check_error();

    ALuint source;
    alGenSources(1, &source);

    alSourcef(source, AL_PITCH, pitch);
    al_check_error();
    alSourcef(source, AL_GAIN, gain);
    al_check_error();
    alSource3f(source, AL_POSITION, 0, 0, 0);
    al_check_error();
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    al_check_error();
    alSourcei(source, AL_LOOPING, AL_FALSE);
    al_check_error();


    ALuint buffer = gen_sound_buffer(path);
    if (!buffer) {
        printf("load %s failed\n", path);
        alDeleteSources(1, &source);
        device = alcGetContextsDevice(context);
        alcMakeContextCurrent(NULL);
        alcDestroyContext(context);
        alcCloseDevice(device);
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
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

typedef struct {
    char* name;
    sprite sp;
    sprite_texture st;
} test_game_object;

void test_game_object_on_start(game_object* obj) {
    test_game_object* self = obj->self;
    printf("%s on start\n", self->name);
}

void test_game_object_on_activate(game_object* obj) {
    test_game_object* self = obj->self;
    printf("%s on activate\n", self->name);
}

void test_game_object_on_update(game_object* obj) {
    if (glfwGetTime() > 3) {
        destory_game_object(obj);
    }
}

void test_game_object_on_render(game_object* obj) {
    test_game_object* self = obj->self;
    transform tran = {
        .position = {sin(glfwGetTime()), 0, 0.1},
        .parent = NULL,
        .scale = {1, 1, 1}
    };
    game_object* cam_obj = find_game_object_by_index(0);
    render_sprite(cam_obj->self, &tran, &self->st, &self->sp);
}

void test_game_object_on_destory(game_object* obj) {
    test_game_object* self = obj->self;
    printf("%s on destory\n", self->name);
}

int main(void)
{
    // sound_test("assets/audio/default/move-self.mp3", 1.0, 1);
    // sound_test("assets/audio/default/move-self.mp3", 1, 0.9);
    // sound_test("assets/audio/default/move-self.mp3", 0.9, 1);
    // sound_test("assets/audio/default/move-self.mp3", 1.9, 0.9);
    // return 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    GLC(glAlphaFunc(GL_GREATER, 0.1));
    GLC(glEnable(GL_ALPHA_TEST));
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // glDisable(GL_DITHER);
    // glDisable(GL_POINT_SMOOTH);
    // glDisable(GL_LINE_SMOOTH);
    // glDisable(GL_POLYGON_SMOOTH);
    // glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
    // glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
    // glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    // glDisable(0x809D);

    // setting up
    setup_anim_system();
    init_sprite_instance();

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

    test_game_object test = {
        .name = "test game object",
        .sp = { {1, 0}, {1, 1, 1, 1}},
    };

    test.st = (sprite_texture){ .per_sprite = {1.0 / 13, 1} };
    init_texture(&test.st.tex, "assets/chess/icy_sea/spritesheet.png", TextureFilterLinear);

    game_object obj = {
        .self = &test,
        .on_start = test_game_object_on_start,
        .on_activate = test_game_object_on_activate,
        .on_update = test_game_object_on_update,
        .on_render = test_game_object_on_render,
        .on_destory = test_game_object_on_destory,
    };

    init_game_object_system();
    create_game_object(&(game_object){
        .self = &cam,
        .on_start = NULL,
        .on_activate = NULL,
        .on_update = NULL,
        .on_render = NULL,
        .on_destory = NULL,
    });
    create_game_object(&obj);

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

        glfwSwapBuffers(app_window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_game_object_system();
    shutdown_anim_system();

    glfwDestroyWindow(app_window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
