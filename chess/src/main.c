#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera.h"
#include "cglm/mat4.h"
#include "cglm/vec2.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"
#include <string.h>
#include "chess_board.h"

#include "memallocate.h"

#define WIDTH 640
#define HEIGHT 640

typedef struct {
    camera cam;
    chess_board board;
    chess* hold;
} Game;

vec2 window_size;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Game* game = glfwGetWindowUserPointer(window);
    glViewport(0, 0, width, height);
    set_camera_ortho_mat4(game->cam.ortho, (vec2){game->cam.canvas[0] * (float)width / height, game->cam.canvas[1]});

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
    float step = 1.0 / 144 * 5;
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

    vec2 index;
    memcpy(index, game->board.tran.position, 2 * sizeof(float));
    index[0] = uv[0] - index[0] + 4;
    index[1] = uv[1] - index[1] + 4;
    glm_vec2_clamp(index, 0, 7);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}

typedef void(*animation_duration_callback)(void*, float);
typedef struct {
    animation_duration_callback callback[2];
    transform start_tran;
    transform* tran;
    f32 time_start, time_duration;
    i32 ended;
} animation_duration;

void animation_duration_end_callback(void* in, float) {
    animation_duration* anim = in;
    anim->ended = 1;
    printf("animation duration end\n");
    // delete_animation_duration(anim);
}

void init_animation_duration(animation_duration* anim, transform* tran, float time_duration, animation_duration_callback callback) {
    anim->callback[0] = callback;
    anim->callback[1] = animation_duration_end_callback;
    anim->tran = tran;
    anim->time_start = -1;
    anim->time_duration = time_duration;
    anim->ended = 0;
}

void activate_animation_duration(animation_duration* anim) {
    anim->time_start = glfwGetTime();
    tran_copy(anim->tran, &anim->start_tran);
}

void animation_duration_start(animation_duration* anim) {
	if (anim->ended) {
	    return;
	}
	float dur = glfwGetTime() - anim->time_start;
    anim->callback[(int)(dur / anim->time_duration)](anim, dur);
}

struct {
    vector(animation_duration) durations;
} animation_duration_system;

void setup_animation_system() {
    animation_duration_system.durations = make_vector();
}

void create_animation_duration(animation_duration* duration) {
    activate_animation_duration(duration);
    vector_pushe(animation_duration_system.durations, *duration);
}

void update_animation_system() {
    for_vector(animation_duration_system.durations, i, 0) {
        animation_duration_start(animation_duration_system.durations + i);
    }
    for_vector(animation_duration_system.durations, i, 0) {
        if (animation_duration_system.durations[i].ended) {
            delete_animation_duration(animation_duration_system.durations + i);
        }
    }
}

void delete_animation_duration(animation_duration* duration) {
    for_vector(animation_duration_system.durations, i, 0) {
        if (animation_duration_system.durations == duration) {
            animation_duration temp = animation_duration_system.durations[i];
            memcpy(animation_duration_system.durations + i, &vector_back(animation_duration_system.durations), sizeof(animation_duration));
            memcpy(&vector_back(animation_duration_system.durations), &temp, sizeof(animation_duration));
            vector_pop(animation_duration_system.durations);
            printf("found duration\n");
            break;
        }
    }
}

void shutdown_animation_system() {
    printf("%ld\n", vector_size(animation_duration_system.durations));
    free_vector(animation_duration_system.durations);
}

void test_animation_duration(void* in, float dur) {
    animation_duration* anim = in;
    float t = sinf(glm_clamp(dur * 5, 0, 1) * 3.14 * 0.5);
    anim->tran->local_position[1] = anim->start_tran.local_position[1] + t;
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

    GLC(glEnable(GL_DEPTH_TEST));
    glDepthFunc(GL_LESS);

    // setting up
    setup_animation_system();
    init_sprite_instance();

    sprite_texture chess_pieces_texture = { .per_sprite = {1.0 / 12, 1} };
    init_texture(&chess_pieces_texture.tex, "chess/classic/spritesheet.png");

    sprite_texture chess_board_texture = { .per_sprite = {1, 1} };
    init_texture(&chess_board_texture.tex, "chess/classic/board.png");

    Game game;
    glfwSetWindowUserPointer(window, &game);

    init_camera(&game.cam, (vec2){5, 5});
    init_chess_board(&game.board);
    game.hold = NULL;
    
    game.board.tran.position[0] = 0;
    game.board.tran.position[1] = 0;

    glClearColor(0.1, 0.1, 0.1, 0);
    glfwSwapInterval(1);

	chess* che = &game.board.grid[8];

    animation_duration anim;
    init_animation_duration(&anim, &che->tran, 0.5, test_animation_duration);
    create_animation_duration(&anim);

    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static int i = 1, count = 0;
		count++;
		if (count % 20 == 0 && i < 8) {
            animation_duration anim;
            init_animation_duration(&anim, &game.board.grid[8 + i].tran, 0.5, test_animation_duration);
            create_animation_duration(&anim);
            count = 0;
            i++;
		}

		update_animation_system();

		render_chess_board(&game.cam, &game.board, &chess_board_texture, &chess_pieces_texture);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    shutdown_animation_system();

    glfwDestroyWindow(window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
