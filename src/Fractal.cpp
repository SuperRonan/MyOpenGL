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

#include <lib/Math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window, bool & reset, bool & use_double, int & max_it)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        use_double = true;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        use_double = false;
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
    {
        ++max_it;
        std::cout << "max it: " << max_it << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
    {
        --max_it;
        std::cout << "max it: " << max_it << std::endl;
    }
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


int fractal_main(GLFWwindow * window)
{
    bool use_double = false;
    using Vertex = lib::Vertex<float>;
    using Camera = lib::Camera<double>;
    using Camera2D = lib::Camera2D<double>;

    using Vector2 = lib::Vector2d;
    using Vector3 = lib::Vector3d;
    using Matrix3 = lib::Matrix3x3d;
    using Matrix4 = lib::Matrix4x4d;

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
    std::string vertex_shader_file, double_vertex_shader_file, fragment_shader_file, double_fragment_shader_file;
    
    vertex_shader_file = shader_folder + "shader1.vert";
    fragment_shader_file = shader_folder + "mandelbrot.frag";
    
    double_vertex_shader_file = shader_folder + "shader1_double.vert";
    double_fragment_shader_file = shader_folder + "mandelbrot_double.frag";
    

    lib::ShaderDesc vertex_shader(vertex_shader_file, GL_VERTEX_SHADER);
    lib::ShaderDesc vertex_shader_double(double_vertex_shader_file, GL_VERTEX_SHADER);
    lib::ShaderDesc fragment_shader(fragment_shader_file, GL_FRAGMENT_SHADER);
    lib::ShaderDesc fragment_shader_double(double_fragment_shader_file, GL_FRAGMENT_SHADER);

    vertex_shader.compile();
    vertex_shader_double.compile();
    fragment_shader.compile();
    fragment_shader_double.compile();
    
    assert(vertex_shader.isCompiled());
    assert(vertex_shader_double.isCompiled());
    assert(fragment_shader.isCompiled());
    assert(fragment_shader_double.isCompiled());

    lib::ProgramDesc program_float(std::move(vertex_shader), std::move(fragment_shader));
    lib::ProgramDesc program_double(std::move(vertex_shader_double), std::move(fragment_shader_double));
    program_float.link();
    program_double.link();

    assert(program_float.isLinked());
    assert(program_double.isLinked());

    std::cout << "Fractal shader1: \n";
    program_float.printAttributes(std::cout);
    program_float.printUniforms(std::cout);
    program_double.printAttributes(std::cout);
    program_double.printUniforms(std::cout);

    Camera camera({ 0.0f, 0.0f, 2.0f });
    lib::MouseHandler mouse_handler(window, lib::MouseHandler::Mode::Position);


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    double t = glfwGetTime(), dt;
    
    Camera2D camera_2D;

    int u_max_it = 500;

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
        processInput(window, reset, use_double, u_max_it);
        if (reset)
        {
            camera_2D.reset();
        }
        mouse_handler.update(dt);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        double aspect_ratio = double(width) / double(height);
        if (width && height)
        {
            // camera -> screen
            const lib::Matrix4x4f mat_P = glm::perspective(glm::radians(mouse_handler.fov), float(aspect_ratio), 0.01f, 1000.0f);
            
            // world to camera
            const lib::Matrix4x4f mat_V = glm::scale(lib::Matrix4x4f(1.f), { aspect_ratio, 1.f, 1.f });

            // model to world
            const lib::Matrix4x4f mat_M = glm::translate(lib::Matrix4x4f(1.f), { 0.f, 0.f, -1.f });

            Matrix3 screen_coords_matrix = lib::scaleMatrix<3, double>({ 1.0 / double(height), 1.0 / double(height) });

            if (mouse_handler.isButtonCurrentlyPressed(GLFW_MOUSE_BUTTON_1))
            {
                camera_2D.move(mouse_handler.deltaPosition<double>());
            }
            else if (mouse_handler.getScroll() != 0)
            {
                Vector2 screen_mouse_pos = mouse_handler.currentPosition<double>();
                camera_2D.zoom(screen_mouse_pos, mouse_handler.getScroll());
            }

            Matrix3 mat_uv_to_fs = screen_coords_matrix * camera_2D.matrix();
            
            lib::ProgramDesc* program = use_double ? &program_double : &program_float;

            glBindVertexArray(VAO);
            program->use();
            program->setUniform("u_V", mat_V);
            program->setUniform("u_P", mat_P);
            program->setUniform("u_M", mat_M);
            program->setUniform("u_max_it", u_max_it);
            if (use_double)
            {
                program_double.setUniform("u_uv_to_fs", mat_uv_to_fs);
            }
            else
            {
                program_float.setUniform("u_uv_to_fs", lib::Matrix3x3f(mat_uv_to_fs));
            }
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
    std::cout << glGetString(GL_RENDERER) << std::endl;

    main_res = fractal_main(window);
    

    glfwTerminate();
    return main_res;
}

