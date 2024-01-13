#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture2D.h"
#include "glfw3.h"
#include "stb_image.h"

f32 vertexs[] =
{
     1.0,  1.0,
    -1.0,  1.0,
     1.0, -1.0,
    -1.0, -1.0,
};

u32 indices[] =
{
    0, 1, 2,
    1, 3, 2,
};

static f32 width = 640;
static f32 height = 640;
static f32 zoom = 4;
static f32 cx = 0;
static f32 cy = 0;
static f32 mouse_x = 0;
static f32 mouse_y = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
}

static ShaderProgram* shader_ptr;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && shader_ptr) {
        shader_ptr->release();
        shader_ptr->create();
        shader_ptr->attach_shader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader_ptr->attach_shader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        shader_ptr->link();
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    zoom -= yoffset * 0.5f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_x = xpos;
    mouse_y = ypos;
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 640, "test", NULL, NULL);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    {
        VertexArray vao;
        VertexBuffer vbo(vertexs, sizeof(f32) * 8);
        IndexBuffer ibo(indices, 6);

        VertexBufferLayout layout;
        layout.add<float>(2);
        vao.apply_buffer_layout(layout);

        ShaderProgram shader;
        shader.attach_shader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader.attach_shader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        shader.link();

        shader_ptr = &shader;

        glClearColor(0.1, 0.1, 0.1, 1);

        while(!glfwWindowShouldClose(window))
        {
            GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            
            shader.bind();
            vao.bind();
            shader.set_2f("resolution", width, height);
            shader.set_2f("mouse_coord", mouse_x / width, mouse_y / height);
            shader.set_2f("center", cx, cy);
            shader.set_1f("time", glfwGetTime());
            shader.set_1f("zoom", zoom);
            GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

            float speed = 0.02;
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                speed  *= 2;
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
                cy -= speed;
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
                cy += speed;
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
                cx -= speed;
            if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
                cx += speed;

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
