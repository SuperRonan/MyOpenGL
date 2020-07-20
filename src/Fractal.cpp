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
#include <lib/Camera2D.h>
#include <lib/MouseHandler.h>
#include <lib/Transforms.h>
#include <lib/StreamOperators.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window, glm::vec3& moving, bool & reset)
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
    reset = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
}

GLFWwindow* createCenteredWindow(int w, int h, const char* name)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* res = glfwCreateWindow(w, h, name, NULL, NULL);
    glfwSetWindowPos(res, (mode->width - w) / 2, (mode->height - h) / 2);
    return res;
}


template <class Float>
int fractal_main(GLFWwindow * window)
{
    using Vertex = lib::Vertex<float>;
    using Camera = lib::Camera<Float>;
    using Camera2D = lib::Camera2D<Float>;

    using Vector2 = glm::vec<2, Float>;
    using Vector3 = glm::vec<3, Float>;
    using Matrix3 = glm::mat<3, 3, Float>;
    using Matrix4 = glm::mat<4, 4, Float>;


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
    std::string vertex_shader_file, fragment_shader_file;
    if constexpr(std::is_same<Float, float>::value)
    {
        vertex_shader_file = shader_folder + "shader1.vert";
        fragment_shader_file = shader_folder + "mandelbrot.frag";
    }
    else
    {
        static_assert(std::is_same<Float, double>::value, "Float must either be float or double.");
        vertex_shader_file = shader_folder + "shader1_double.vert";
        fragment_shader_file = shader_folder + "mandelbrot_double.frag";
    }

    lib::ShaderDesc vertex_shader(vertex_shader_file, GL_VERTEX_SHADER);
    lib::ShaderDesc fragment_shader(fragment_shader_file, GL_FRAGMENT_SHADER);

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
    lib::MouseHandler mouse_handler(window, lib::MouseHandler::Mode::Position);


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    double t = glfwGetTime(), dt;

    Camera2D camera_2D;


    while (!glfwWindowShouldClose(window))
    {
        {
            double new_t = glfwGetTime();
            dt = new_t - t;
            t = new_t;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();

        glm::vec3 zqsd;
        bool reset;
        processInput(window, zqsd, reset);
        if (reset)
        {
            camera_2D.reset();
        }
        mouse_handler.update(dt);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        Float aspect_ratio = Float(width) / Float(height);
        if (width && height)
        {
            // camera -> screen
            glm::mat4 mat_P;
            mat_P = glm::perspective(glm::radians(mouse_handler.fov), float(aspect_ratio), 0.01f, 1000.0f);
            //mat_P = glm::ortho(0.0f, float(w), 0.0f, float(h), 0.01f, 100.0f);

            // world to camera
            const glm::mat4 mat_V = glm::scale(Matrix4(1.f), { aspect_ratio, 1.f, 1.f });

            // model to world
            const glm::mat4 mat_M = glm::translate(Matrix4(1.f), { 0.f, 0.f, -1.f });

            Matrix3 screen_coords_matrix = lib::scaleMatrix<3, Float>({ 1.0f / Float(height), 1.0f / Float(height) });

            if (mouse_handler.isButtonCurrentlyPressed(GLFW_MOUSE_BUTTON_1))
            {
                camera_2D.move(mouse_handler.deltaPosition<Float>());
            }
            else if (mouse_handler.getScroll() != 0)
            {
                Vector2 screen_mouse_pos = mouse_handler.currentPosition<Float>();
                camera_2D.zoom(screen_mouse_pos, mouse_handler.getScroll());
            }

            Matrix3 mat_uv_to_fs = screen_coords_matrix * camera_2D.matrix();
            
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
    return 0;
}

int main()
{
    
    int main_res = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    if (!gladLoadGL())
    {
        std::cout << "Unable to load OpenGL functions!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, w, h);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    std::cout << glGetString(GL_VERSION) << std::endl;
    

    main_res = fractal_main<float>(window);
    

    glfwTerminate();
    return main_res;
}

