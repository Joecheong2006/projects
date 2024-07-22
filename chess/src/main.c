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

#include <AL/al.h>
#include <AL/alext.h>
#include <sndfile.h>

#include "memallocate.h"

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
    window_control render;
} controller;

typedef struct {
    GLFWwindow* window;
    int width, height;
} window_state;

void close_application(window_state* window) {
    glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

typedef struct {
    camera cam;
    chess_board board;
    chess* hold;
    vec2 cursor_index;
    struct {
        sprite_texture chess_board_tex, chess_pieces_tex;
    } chess_theme;
    window_state win_state;
} Game;

void game_window_resize_callback(void* owner, int width, int height) {
    Game* game = owner;
    glViewport(0, 0, width, height);
    set_camera_ortho_mat4(game->cam.projection, (vec2){width, height});

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
    glm_mat4_mul(game->cam.projection, game->cam.view, m);
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
                game->hold = NULL;
                return;
            }

            if ((game->board.round % 2 == 0 && game->hold->is_white) || (game->board.round % 2 && !game->hold->is_white)) {
                game->hold = NULL;
                return;
            }

            int legal = game->hold->is_legal_move(&game->board, pre_index, game->cursor_index);
            if (!legal) {
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
                    game->hold = NULL;
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

void game_render_callback(void* owner) {
    Game* game = owner;
	render_chess_board(&game->cam, &game->board, &game->chess_theme.chess_board_tex, &game->chess_theme.chess_pieces_tex);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    controller* c = glfwGetWindowUserPointer(window);
    if (c->render.resize_callback) {
        c->render.resize_callback(c->owner, width, height);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    controller* c = glfwGetWindowUserPointer(window);
    if (c->input.key_callback) {
        c->input.key_callback(c->owner, key, scancode, action, mods);
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    controller* c = glfwGetWindowUserPointer(window);
    if (c->input.cursor_pos_callback) {
        c->input.cursor_pos_callback(c->owner, xpos, ypos);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    controller* c = glfwGetWindowUserPointer(window);
    if (c->input.mouse_button_callback) {
        c->input.mouse_button_callback(c->owner, button, action, mods);
    }
}

char* get_al_error(ALCenum err) {
    switch (err) {
      case AL_NO_ERROR: return "AL_NO_ERROR";
      case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
      case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
      case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
      /* ... */
      default:
        return "Unknown error code";
    }
}

void al_check_error() {
    ALCenum error;

    error = alGetError();
    if (error != AL_NO_ERROR) {
        printf("al error %d %s\n", error, get_al_error(error));
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

static inline ALenum to_al_format(short channels, short samples)
{
        bool stereo = (channels > 1);

        switch (samples) {
        case 16:
                if (stereo)
                        return AL_FORMAT_STEREO16;
                else
                        return AL_FORMAT_MONO16;
        case 8:
                if (stereo)
                        return AL_FORMAT_STEREO8;
                else
                        return AL_FORMAT_MONO8;
        default:
                return -1;
        }
}

#include <limits.h>

ALuint gen_sound_buffer(const char* file_name) {
    SF_INFO sf_info;
    SNDFILE* sndfile = sf_open(file_name, SFM_READ, &sf_info);
    if (!sndfile) {
        return 0;
    }
    if (sf_info.frames < 1 || sf_info.frames > (sf_count_t)(INT_MAX / sizeof(short)) / sf_info.channels) {
        sf_close(sndfile);
        return 0;
    }

    ALenum format = AL_NONE;
    if (sf_info.channels == 1) {
        format = AL_FORMAT_MONO16;
    }
    else if (sf_info.channels == 2) {
        format = AL_FORMAT_STEREO16;
    }
    else if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
        if (sf_info.channels == 3) {
            format = AL_FORMAT_BFORMAT2D_16;
        }
        else if (sf_info.channels == 4) {
            format = AL_FORMAT_BFORMAT3D_16;
        }
    }

    if (!format) {
        return 0;
    }

    short* buf = malloc(sf_info.frames * sf_info.channels * sizeof(short));
    if (!buf) {
        return 0;
    }

    sf_count_t frames_count = sf_readf_short(sndfile, buf, sf_info.frames);
    if (frames_count < 1) {
        free(buf);
        return 0;
    }
    u64 size = frames_count * sf_info.channels * sizeof(short);
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, buf, size, sf_info.samplerate);
    free(buf);
    sf_close(sndfile);

    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        alDeleteBuffers(1, &buffer);
        return 0;
    }

    return buffer;
}

void bonk() {
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

    ALfloat listenerOri[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    alListener3f(AL_POSITION, 0, 0, 0.0f);
    al_check_error();
    alListener3f(AL_VELOCITY, 0, 0, 0);
    al_check_error();
    alListenerfv(AL_ORIENTATION, listenerOri);
    al_check_error();

    ALuint source;
    alGenSources(1, &source);
    
    alSourcef(source, AL_PITCH, 1);
    al_check_error();
    alSourcef(source, AL_GAIN, 1);
    al_check_error();
    alSource3f(source, AL_POSITION, 0, 0, 0);
    al_check_error();
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    al_check_error();
    alSourcei(source, AL_LOOPING, AL_FALSE);
    al_check_error();


    // ALuint buffer = gen_sound_buffer("assets/audio/bonk.wav");
    ALuint buffer = gen_sound_buffer("assets/audio/bonk.mp3");
    // ALuint buffer = gen_sound_buffer("assets/audio/test.wav");

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
int main(void)
{
    bonk();

    // wav_header wh;
    // read_wav_info("assets/audio/bonk.wav", &wh);
    // printf("%d %d %d\n", to_al_format(wh.num_channels, wh.bits_per_sample), wh.data_size, wh.sample_rate);
    // free(wh.data);

    return 0;
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
    // GLC(glAlphaFunc(GL_GREATER, 0.1));
    // GLC(glEnable(GL_ALPHA_TEST));
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    // setting up
    setup_anim_system();
    init_sprite_instance();

    Game game;
    game.win_state.window = app_window;

    game.chess_theme.chess_pieces_tex = (sprite_texture){ .per_sprite = {1.0 / 12, 1} };
    // init_texture(&game.chess_theme.chess_pieces_tex.tex, "chess/classic/spritesheet.png");
    init_texture(&game.chess_theme.chess_pieces_tex.tex, "assets/chess/icy_sea/spritesheet.png");

    game.chess_theme.chess_board_tex = (sprite_texture){ .per_sprite = {1, 1} };
    // init_texture(&game.chess_theme.chess_board_tex.tex, "chess/classic/board.png");
    init_texture(&game.chess_theme.chess_board_tex.tex, "assets/chess/icy_sea/board.png");


    controller con = (controller){
        .owner = &game,
        .input.cursor_pos_callback = game_cursor_pos_callback,
        .input.key_callback = game_key_callback,
        .input.mouse_button_callback = game_mouse_button_callback,
        .render.resize_callback = game_window_resize_callback,
        .render.render_callback = game_render_callback,
    };

    glfwSetWindowUserPointer(app_window, &con);

    init_camera(&game.cam, (vec2){WIDTH, HEIGHT});
    init_chess_board(&game.board);
    game.hold = NULL;
    
    game.board.tran.position[0] = 0;
    game.board.tran.position[1] = 0;
    printf("white\n");

    anim_position_slide chess_move_anim;
    init_anim_position_slide(&chess_move_anim, (vec3){0, 1, 0}, chess_move_anim_callback);

    glClearColor(0.0, 0.1, 0.1, 0);

    while(!glfwWindowShouldClose(app_window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float step = 1.0 / 144 * 4;
        if (glfwGetKey(app_window, GLFW_KEY_UP) == GLFW_PRESS) {
            translate_camera(&game.cam, (vec3){0, step, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            translate_camera(&game.cam, (vec3){0, -step, 0});
        }
        if (glfwGetKey(app_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            translate_camera(&game.cam, (vec3){-step, 0, 0});
        }
        else if (glfwGetKey(app_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            translate_camera(&game.cam, (vec3){step, 0, 0});
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
		con.render.render_callback(con.owner);

        glfwSwapBuffers(app_window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_anim_system();

    glfwDestroyWindow(app_window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
