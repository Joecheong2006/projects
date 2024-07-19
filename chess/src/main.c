#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera.h"
#include "cglm/mat4.h"
#include "cglm/vec2.h"
#include "cglm/vec3.h"
#include "string.h"
#include "opengl_object.h"
#include "sprite.h"
#include "stb_image.h"
#include <string.h>
#include "chess_board.h"

#include "memallocate.h"

#define WIDTH 640
#define HEIGHT 640

#define PI 3.14159265359

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

typedef struct anim_duration anim_duration;
typedef void(*anim_duration_callback)(anim_duration*, float);
struct anim_duration {
    anim_duration_callback callback[2];
    void* in;
    f32 time_start, time_duration;
    i32 ended;
};

void anim_duration_end_callback(anim_duration* in, float) {
    anim_duration* anim = in;
    anim->ended = 1;
    printf("animation duration end\n");
    // delete_animation_duration(anim);
}

void init_anim_duration(anim_duration* anim, void* in, float time_duration, anim_duration_callback callback) {
    anim->callback[0] = callback;
    anim->callback[1] = anim_duration_end_callback;
    anim->in = in;
    anim->time_start = -1;
    anim->time_duration = time_duration;
    anim->ended = 0;
}

void activate_anim_duration(anim_duration* anim) {
    anim->time_start = glfwGetTime();
}

void anim_duration_start(anim_duration* anim) {
	if (anim->ended) {
	    return;
	}
	float dur = glfwGetTime() - anim->time_start;
    anim->callback[(int)(dur / anim->time_duration)](anim, dur);
}

typedef struct anim_position_slide anim_position_slide;
typedef void(*anim_position_slide_callback)(anim_position_slide*, float);
struct anim_position_slide {
    vec3 translation, start, end;
    vec3* target;
    anim_position_slide_callback callback;
};

void anim_position_duration_callback(anim_duration* anim, float dur) {
    anim_position_slide* slide = anim->in;
    slide->callback(slide, dur / anim->time_duration);
}

void init_anim_position_slide(anim_position_slide* slide, vec3* target_position, vec3 translation, anim_position_slide_callback callback) {
    slide->callback = callback;
    slide->target = target_position;
    glm_vec3_copy(*slide->target, slide->start);
    glm_vec3_copy(translation, slide->translation);
    slide->end[0] = slide->start[0] + slide->translation[0];
    slide->end[1] = slide->start[1] + slide->translation[1];
    slide->end[2] = slide->start[2] + slide->translation[2];
}

void init_anim_position_slide_duration(anim_duration* anim, anim_position_slide* slide, float time_duration) {
    init_anim_duration(anim, slide, time_duration, anim_position_duration_callback);
}

typedef struct {
    vector(anim_duration) durations;
} anim_duration_system;

anim_duration_system* get_anim_duration_system() {
    static anim_duration_system instance;
    return &instance;
}

void setup_anim_system() {
    get_anim_duration_system()->durations = make_vector();
}

void create_anim_duration(anim_duration* duration) {
    activate_anim_duration(duration);
    vector_pushe(get_anim_duration_system()->durations, *duration);
}

void delete_anim_duration(anim_duration* duration);
void update_anim_system() {
    anim_duration_system* system = get_anim_duration_system();
    for_vector(system->durations, i, 0) {
        anim_duration_start(system->durations + i);
    }
    for_vector(system->durations, i, 0) {
        if (system->durations[i].ended) {
            delete_anim_duration(system->durations + i);
        }
    }
}

void delete_anim_duration(anim_duration* duration) {
    anim_duration_system* system = get_anim_duration_system();
    for_vector(system->durations, i, 0) {
        if (system->durations == duration) {
            anim_duration temp = system->durations[i];
            memcpy(system->durations + i, &vector_back(system->durations), sizeof(anim_duration));
            memcpy(&vector_back(system->durations), &temp, sizeof(anim_duration));
            vector_pop(system->durations);
            break;
        }
    }
}

void shutdown_anim_system() {
    anim_duration_system* system = get_anim_duration_system();
    printf("%ld\n", vector_size(system->durations));
    free_vector(system->durations);
}

void test_anim_duration(anim_position_slide* slide, float dur) {
    const float t = sinf(dur * PI * 0.5);
    (*slide->target)[1] = glm_lerp(slide->start[1], slide->end[1], t);
}

int main(void)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);

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
    glDepthFunc(GL_LESS);
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

    init_camera(&game.cam, (vec2){5, 5});
    init_chess_board(&game.board);
    game.hold = NULL;
    
    game.board.tran.position[0] = 0;
    game.board.tran.position[1] = 0;

    glClearColor(0.0, 0.1, 0.1, 0);
    glfwSwapInterval(1);

	// chess* che = &game.board.grid[8];
    // anim_duration anim;
    // anim_position_slide slide;
    // init_anim_position_slide(&slide, &che->tran.local_position, (vec3){0, 1, 0}, test_anim_duration);
    // init_anim_position_slide_duration(&anim, &slide, 0.25);
    // init_anim_duration(&anim, &che->tran, 0.25, test_anim_duration);
    // create_anim_duration(&anim);

    anim_position_slide slides[8];

    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static int i = 0, count = 0, p = 0;
		count++;

		if (p == 0 && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		    i = 0;
		}
		if (count % 20 == 0 && i < 8) {
		    p = 1;
            anim_duration anim;
            init_anim_position_slide(&slides[i], &game.board.grid[8 + i].tran.local_position, (vec3){0, 1, 0}, test_anim_duration);
            init_anim_position_slide_duration(&anim, &slides[i], 0.25);
            create_anim_duration(&anim);
            count = 0;
            i++;
		}
		if (i == 8) {
		    p = 0;
		}

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
