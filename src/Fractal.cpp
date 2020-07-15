#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <exception>
#include <vector>
#include <string>
#include <type_traits>
#include <glm/glm.hpp>
#include <lib/ShaderDesc.h>
#include <lib/ProgramDesc.h>
#include <lib/Vertex.h>
#include <lib/Log.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <lib/Camera.h>
#include <lib/MouseHandler.h>
#include <lib/Transforms.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window, glm::vec3& moving, float& fov)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    moving = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moving.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moving.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moving.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moving.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        moving.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        moving.z -= 1.0f;
}

GLFWwindow* createCenteredWindow(int w, int h, const char* name)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* res = glfwCreateWindow(w, h, name, NULL, NULL);
    glfwSetWindowPos(res, (mode->width - w) / 2, (mode->height - h) / 2);
    return res;
}


float scroll = 0.0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scroll = (float)yoffset;
}

int main()
{
    using Vertex = lib::Vertex<float>;
    using Camera = lib::Camera<float>;
    int main_res = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int w = 1920, h = 1080;

    GLFWwindow* window = createCenteredWindow(w, h, "Fractal go Brrrrrr...");
    if (window == NULL)
    {
        std::cerr << "Could not create the window:\n" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Could not initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, w, h);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    lib::MouseHandler mouse_handler(window, lib::MouseHandler::Mode::Position);


    std::vector<Vertex> vertices = {
        {{1.f,  1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // top right
        {{1.f, -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // bottom right
        {{-1.f, -1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // bottom left
        {{-1.f,  1.f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // top left 
    };
    std::vector<unsigned int> indices = {
        0, 3, 1,
        1, 3, 2,
    };

    GLuint VAO; glGenVertexArrays(1, &VAO);
    GLuint VBO; glGenBuffers(1, &VBO);
    GLuint EBO; glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::positionOffset());
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::normalOffset());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, Vertex::stride(), (void*)Vertex::uvOffset());
    glEnableVertexAttribArray(2);

    std::string shader_folder = "../shaders/";
    lib::ShaderDesc vertex_shader(shader_folder + "shader1.vert", GL_VERTEX_SHADER);
    lib::ShaderDesc fragment_shader(shader_folder + "mandelbrot.frag", GL_FRAGMENT_SHADER);

    vertex_shader.compile();
    fragment_shader.compile();

    assert(vertex_shader.isCompiled());
    assert(fragment_shader.isCompiled());

    lib::ProgramDesc program(&vertex_shader, &fragment_shader);
    program.link();

    assert(program.isLinked());

    std::cout << "Fractal shader1: \n";
    program.printAttributes(std::cout);
    program.printUniforms(std::cout);

    Camera camera({ 0.0f, 0.0f, 2.0f });

    glfwSetScrollCallback(window, scroll_callback);
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    double t = glfwGetTime(), dt;

    glm::mat3 zoom_matrix(1.f);
    glm::vec2 zoom_t(0.f);
    glm::vec2 translate(0.0f);
    float zoom = 1.0f;

    while (!glfwWindowShouldClose(window))
    {
        scroll = 0;
        {
            double new_t = glfwGetTime();
            dt = new_t - t;
            t = new_t;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();

        glm::vec3 zqsd;

        processInput(window, zqsd, mouse_handler.fov);
        mouse_handler.update(dt);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = float(width) / float(height);
        if (width && height)
        {
            // camera -> screen
            glm::mat4 mat_P;
            mat_P = glm::perspective(glm::radians(mouse_handler.fov), aspect_ratio, 0.01f, 1000.0f);
            //mat_P = glm::ortho(0.0f, float(w), 0.0f, float(h), 0.01f, 100.0f);

            // world to camera
            const glm::mat4 mat_V = glm::scale(glm::mat4(1.f), { aspect_ratio, 1.f, 1.f });

            // model to world
            const glm::mat4 mat_M = glm::translate(glm::mat4(1.f), { 0.f, 0.f, -1.f });

            glm::mat3 screen_coords_matrix = lib::scaleMatrix<3, float>({ 1.0f / float(height), 1.0f / float(height) });

            if (mouse_handler.isButtonCurrentlyPressed(GLFW_MOUSE_BUTTON_1))
            {
                glm::vec2 delta_pos = mouse_handler.deltaPosition<float>();
                glm::vec3 h_delta = (lib::scaleMatrix<3, float>({ zoom, zoom }) * glm::vec3(delta_pos, 1.0f));
                translate -= glm::vec2(h_delta.x, h_delta.y);
            }
            else if (scroll != 0)
            {
                float ds = 0.05f;
                float mult = scroll > 0 ? (1.0f / (1.0f + scroll * ds)) : ((-scroll * ds + 1.0f));
                zoom *= mult;
                glm::mat3 t_mat = lib::translateMatrix<3, float>(translate);
                glm::mat3 t_mat_inv = lib::translateMatrix<3, float>(-translate);

                glm::vec2 new_zoom_t = mouse_handler.currentPosition<float>();
                //new_zoom_t = { 300, 200 };
                glm::mat3 mouse_T_matrix = lib::translateMatrix<3, float>(-new_zoom_t);
                glm::mat3 mouse_T_inv_matrix = lib::translateMatrix<3, float>(new_zoom_t);

                glm::mat3 zoom_mat = lib::scaleMatrix<3, float>({ zoom, zoom });
                glm::mat3 zoom_mat_inv = lib::scaleMatrix<3, float>({ 1.f/zoom, 1.f/zoom });
                
                zoom_matrix = mouse_T_inv_matrix * zoom_mat * mouse_T_matrix;
                zoom_t = new_zoom_t;

                glm::vec2 mouse_T_pos = zoom_matrix * glm::vec3(new_zoom_t, 1.f);
                int i = 0;
            }

            glm::mat3 mat_uv_to_fs = screen_coords_matrix * lib::translateMatrix<3, float>(translate) * zoom_matrix ;

            glClearColor(0.2, 0.3, 0.3, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glBindVertexArray(VAO);
            program.use();
            program.setUniform("u_V", mat_V);
            program.setUniform("u_P", mat_P);
            program.setUniform("u_M", mat_M);
            program.setUniform("u_uv_to_fs", mat_uv_to_fs);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


            glBindVertexArray(0);
            lib::ProgramDesc::useNone();
        }
    }

    glfwTerminate();
    return main_res;
}
