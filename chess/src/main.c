#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shader_program.h"
#include "string.h"
#include "opengl_object.h"
#include "stb_image.h"
#include "cglm/cglm.h"

#include "memallocate.h"

#define WIDTH 640
#define HEIGHT 640

typedef struct {
    char* title;
} window_data;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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

typedef struct {
    vec2 position;
    vec2 scale;
} transform2d;

typedef struct {
    vec2 per_sprite;
    texture tex;
} sprite_texture;

typedef struct {
    transform2d tran;
    vec2 sprite_index;
} sprite;

typedef struct {
    mat4 ortho;
} camera2d;

void init_camera(camera2d* cam) {
    glm_ortho(-10, 10, -10, 10, -1, 1, cam->ortho);
}

void get_camera_ortho_mat4(mat4 ortho, vec2 canvas) {
    glm_ortho(-canvas[0], canvas[0], -canvas[1], canvas[1], -1, 1, ortho);
}

struct {
    vertex_array vao;
    vertex_buffer vbo;
    index_buffer ibo;
    u32 shader;
} sprite_instance;

void init_sprite_instance() {
    GLC(init_vertex_array(&sprite_instance.vao));
    sprite_instance.vbo = (vertex_buffer){
        .vertices = (f32[]){
             0.5,  0.5,  0.0, 1.0, 1.0,
            -0.5,  0.5,  0.0, 0.0, 1.0,
             0.5, -0.5,  0.0, 1.0, 0.0,
            -0.5, -0.5,  0.0, 0.0, 0.0,
        }
    };
    GLC(init_vertex_buffer(&sprite_instance.vbo, 5, 4, GL_STATIC_DRAW));
    GLC(vertex_array_add_attribute(&sprite_instance.vao, &sprite_instance.vbo, 3, GL_FLOAT));
    GLC(vertex_array_add_attribute(&sprite_instance.vao, &sprite_instance.vbo, 2, GL_FLOAT));

    sprite_instance.ibo = (index_buffer){
        .index = (u32[]){
            0, 1, 2,
            2, 1, 3,
        }
    };
    GLC(init_index_buffer(&sprite_instance.ibo, 6, GL_STATIC_DRAW));

    shader_program shader = parse_shader("res/shaders/Basic.shader");
    GLC(sprite_instance.shader = create_shader(shader.vertex, shader.fragment));
    shader_program_free(&shader);

    glBindVertexArray(0);
}

void render_sprite(camera2d* cam, sprite_texture* sprite_tex, sprite* sp) {
	GLC(glUseProgram(sprite_instance.shader));

	glActiveTexture(GL_TEXTURE0 + sprite_tex->tex.slot);
	GLC(glBindTexture(GL_TEXTURE_2D, sprite_tex->tex.id));

    int location;
    GLC(location = glGetUniformLocation(sprite_instance.shader, "tex"));
    GLC(glUniform1i(location, sprite_tex->tex.slot));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "per_sprite"));
    GLC(glUniform2f(location, sprite_tex->per_sprite[0], sprite_tex->per_sprite[1]));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "sprite_index"));
    GLC(glUniform2f(location, sp->sprite_index[0], sp->sprite_index[1]));

    mat4 m, trans, scale;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){sp->tran.position[0], sp->tran.position[1], 0});

    glm_mat4_identity(scale);
    glm_scale(scale, (vec3){sp->tran.scale[0], sp->tran.scale[1], 0});

    glm_mat4_identity(m);
    glm_mat4_mul(cam->ortho, trans, m);
    glm_mat4_mul(m, scale, m);

    GLC(location = glGetUniformLocation(sprite_instance.shader, "view"));
    GLC(glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]));
    
	GLC(glBindVertexArray(sprite_instance.vao.id));
    GLC(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	glBindVertexArray(0);
}

void init_chess(sprite* sp, vec2 position, vec2 sprite_index) {
    sp->tran = (transform2d){
        .position = { position[0], position[1] },
        .scale = {1, 1}
    };
    glm_vec2_copy(sprite_index, sp->sprite_index);
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

    init_sprite_instance();

    sprite_texture chess_pieces = {
        .per_sprite = {0.1, 1},
    };
    init_texture(&chess_pieces.tex, "chess/classic/spritesheet.png");

    sprite_texture chess_board = { .per_sprite = {1, 1} };
    init_texture(&chess_board.tex, "chess/classic/board.png");

    camera2d cam;
    get_camera_ortho_mat4(cam.ortho, (f32[]){7, 7});

    glClearColor(0.1, 0.1, 0.1, 0);

    while(!glfwWindowShouldClose(window))
    {
		glClear(GL_COLOR_BUFFER_BIT);

		sprite board = {
		    .tran = {
		        .position = {-0.5, 0.5},
		        .scale = {8, 8}
    		},
		    .sprite_index = {0, 0}
		};
		render_sprite(&cam, &chess_board, &board);

		for (int i = 0; i < 8; i++) {
    		sprite chess;
    		init_chess(&chess, (vec2){i - 4, 3}, (vec2){3, 0});
    		render_sprite(&cam, &chess_pieces, &chess);
		}

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(sprite_instance.shader);
    glfwDestroyWindow(window);
    glfwTerminate();

    CHECK_MEMORY_LEAK();

    return 0;
}
