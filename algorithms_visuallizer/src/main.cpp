#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture2D.h"
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "map.h"

#include <cmath>
#include <random>
#include <algorithm>
#include <chrono>
#include <utility>

#define WIDTH 960
#define HEIGHT 640

static f32 vertex[] = {
     1,  1,
    -1,  1,
     1, -1,
    -1, -1,
};

static u32 index[] = {
    1, 0, 2,
    1, 3, 2,
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

typedef int sort_element_type;
typedef std::vector<sort_element_type> sort_type;

void swap(sort_element_type* a, sort_element_type* b) {
    sort_element_type temp = *a;
    *a = *b;
    *b = temp;
}

void shuffle(sort_type& arr, i32 rs, i32 re) {
    int i = rs;
    std::uniform_int_distribution<std::mt19937::result_type> dist(rs, re);
    std::generate(std::begin(arr), std::end(arr), [&](){ return i++; });
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(arr.begin(), arr.end(), std::default_random_engine(seed));
}

struct algorithm {
    algorithm(sort_type& sort): target(sort), target_len(sort.size()), trace(0)
    {}
    virtual ~algorithm() {}

    virtual void solve() = 0;

    static bool complate(algorithm* algo) {
        for (i32 i = 0; i < algo->target_len - 1; i++) {
            if (algo->target[i] > algo->target[i + 1]) {
                return false;
            }
        }
        return true;
    }
    sort_type& target;
    const i32 target_len;
    i32 trace;
};

struct Insertion : public algorithm {
    Insertion(sort_type& sort): algorithm(sort) {
        curr = 1;
        prew = curr;
        trace = prew;
    }

    virtual void solve() override {
        if (curr >= target_len)
            return;
        trace = prew;
        if (target[prew] < target[prew - 1]) {
            swap(&target[prew - 1], &target[prew]);
        }
        prew--;
        if (prew <= 0) {
            prew = ++curr;
        }
    }

    i32 prew, curr;
};

struct Bubble : public algorithm {
    Bubble(sort_type& sort): algorithm(sort) {
        curr = 0;
        next = curr;
        trace = next;
    }

    virtual void solve() override {
        if (curr >= target_len)
            return;
        trace = next;
        if (target[curr] > target[next]) {
            swap(&target[curr], &target[next]);
        }
        next++;
        if (next >= target_len) {
            next = ++curr;
        }
    }

    i32 next, curr;
};

#define APPLY_SOLUTION(name) { #name, Solution::name },

#define DEFINE_SOLUTION(...)\
    enum Solution {\
        __VA_ARGS__\
    };\
    const std::vector<std::pair<const char*, Solution>> solutions = {\
        MAP(APPLY_SOLUTION, __VA_ARGS__)\
    };\

#define SET_SOLUTION_CASE(C)\
    case C:\
        solver = new struct C(vec);\
     break;\

#define SET_SOLUTIONS_TO_SOLVER(...)\
    MAP(SET_SOLUTION_CASE, __VA_ARGS__)

#define SOLUTIONS Bubble, Insertion

DEFINE_SOLUTION(SOLUTIONS);

i32 get_solution_index(void* solution) {
    for (i32 i = 0; i < (i32)sizeof(solution); i++) {
        if (solution == solutions[i].first) {
            return i;
        }
    }
    return -1;
}

static struct settings {
    i32 length = 200;
    i32 step = 30;
} settings;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "test", NULL, NULL);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);

    GLCALL(glEnable(GL_BLEND));
    GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    {
        VertexArray vao;
        IndexBuffer ibo(index, sizeof(index));
        VertexBuffer vbo(vertex, sizeof(vertex));
        VertexBufferLayout layout;
        layout.add<f32>(2);
        vao.applyBufferLayout(layout);

        ShaderProgram shader;
        shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default.vert");
        shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
        shader.link();

        vao.unbind();
        ibo.unbind();
        vbo.unbind();
        shader.unbind();

        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO io = ImGui::GetIO();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        glClearColor(0.1, 0.1, 0.1, 1);

        struct settings new_settings;

        std::vector<int> vec(new_settings.length);
        shuffle(vec, 1, new_settings.length);

        algorithm* solver = nullptr;
        const char* current_solution = solutions[0].first;

        while(!glfwWindowShouldClose(window))
        {
            GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            vao.bind();
            shader.bind();

            if (solver) {
                bool complated = algorithm::complate(solver);

                if (!complated) {
                    for (i32 i = 0; i < settings.step; i++)
                        solver->solve();
                }

                f32 w = 2.0 / settings.length;
                for (i32 i = 0; i < settings.length; i++) {
                    f32 h = vec[i] * 1.0 / (settings.length);
                    if (solver->trace != i)
                        shader.set3f("color", 1.4 - h, 0.6, h);
                    else if (!complated)
                        shader.set3f("color", 1, 0, 0);
                    glm::mat4 model = glm::mat4(1);
                    model = glm::translate(model, glm::vec3(w * i - 1 + 1.0 / settings.length, h - 1, 0));
                    model = glm::scale(model, glm::vec3(w * 0.5f, h, 1));
                    shader.setMat4("model", model);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }


            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("settings");

            if (ImGui::Button("start")) {
                if (solver) {
                    delete solver;
                    std::cout << "hi";
                }
                settings = new_settings;
                vec.resize(settings.length);
                switch (get_solution_index((void*)current_solution)) {
                    SET_SOLUTIONS_TO_SOLVER(SOLUTIONS);
                    default:
                        std::cout << "not match any solution\n";
                        assert(false);
                        break;
                }
                shuffle(vec, 1, settings.length);
            }

            ImGui::SliderInt("length", &new_settings.length, 100, 700);
            ImGui::SliderInt("step", &new_settings.step, 1, 240);

            if (ImGui::BeginCombo("solutions", current_solution)) {
                for (i32 n = 0; n < (i32)solutions.size(); n++)
                {
                    bool is_selected = (current_solution == solutions[n].first);
                    if (ImGui::Selectable(solutions[n].first, is_selected)) {
                        current_solution = solutions[n].first;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::End();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        if (solver)
            delete solver;
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
