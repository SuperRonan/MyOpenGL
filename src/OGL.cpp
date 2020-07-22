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


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/// <summary>
/// Fills the vertices and indices arrays to make a cube, centered on 0, of size 1
/// </summary>
template <class Float>
void makeCube(std::vector<lib::Vertex<Float>>& vertices, std::vector<GLuint>& indices)
{
    //        (0,0)                      (h,0)    
    //          x0------------------------x1
    //         /|                        /|
    //        / |                       / |
    //       /  |                      /  |
    //      /   |                     /   |
    //(0,h)x3------------------------x2   | (h,h)
    //     |    |        +y          |    |
    //     |    |        ^           |    |
    //     |    |        |           |    |
    //     |    |        |           |    |
    //     |    | (h,0)  0--->+x     |    |
    //     |    x4------/------------|----x5 (1,h)  
    //     |   /       /             |   / 
    //     |  /       +z             |  /  
    //     | /                       | /   
    //     |/                        |/    
    //(h,h)x7------------------------x6 (1,1)

    const auto makeVertex = [](Float x, Float y, Float z, Float u, Float v) 
    {return lib::Vertex<Float>({ x, y, z }, { x, y, z }, { u, v }); };

    // half
    const Float h = Float(0.5);
    vertices = {
        makeVertex(-h, h, -h, 0, 0), // x0
        makeVertex(h, h, -h, h, 0), // x1
        makeVertex(h, h, h, h, h), // x2
        makeVertex(-h, h, h, 0, h), // x3
        makeVertex(-h, -h, -h, h, 0), // x4
        makeVertex(h, -h, -h, 1, h), // x5
        makeVertex(h, -h, h, 1, 1), // x6
        makeVertex(-h, -h, h, h, h), // x7
    };

    indices.resize(0);
    const auto addFace = [&indices](GLuint v0, GLuint v1, GLuint v2, GLuint v3)
    {
        indices.push_back(v0);
        indices.push_back(v1);
        indices.push_back(v2);

        indices.push_back(v2);
        indices.push_back(v3);
        indices.push_back(v0);
    };

    addFace(0, 3, 2, 1); // up (+y)
    addFace(0, 1, 5, 4); // back (-z)
    addFace(0, 4, 7, 3); // left (-x)
    addFace(6, 7, 4, 5); // bottom (-y)
    addFace(6, 2, 3, 7); // front (+z)
    addFace(6, 5, 1, 2); // right (+x)
}

void processInput(GLFWwindow* window, glm::vec3 & moving, float & fov)
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

GLFWwindow* createCenteredWindow(int w, int h, const char * name)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode * mode = glfwGetVideoMode(monitor);
    GLFWwindow *res = glfwCreateWindow(w, h, name, NULL, NULL);
    glfwSetWindowPos(res, (mode->width - w) / 2, (mode->height - h) / 2);
    return res;
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
    
    GLFWwindow* window = createCenteredWindow(w, h, "OpenGL go Brrrrrr...");
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
    lib::MouseHandler mouse_handler(window);


    std::vector<Vertex> vertices = {
        {{0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // top right
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},  // bottom right
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // bottom left
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // top left 
    };
    std::vector<unsigned int> indices = {
        0, 1, 3,
        1, 3, 2,
    };

    makeCube(vertices, indices);
    
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
    lib::ShaderDesc fragment_shader(shader_folder + "shader1.frag", GL_FRAGMENT_SHADER);

    vertex_shader.compile();
    fragment_shader.compile();

    assert(vertex_shader.isCompiled());
    assert(fragment_shader.isCompiled());

    lib::ProgramDesc program(&vertex_shader, &fragment_shader);
    program.link();
    
    assert(program.isLinked());

    std::cout << "Program shader1: \n";
    program.printAttributes(std::cout);
    program.printUniforms(std::cout);

    Camera camera({ 0.0f, 0.0f, 2.0f });

    // camera -> screen
    glm::mat4 mat_P = glm::perspective(glm::radians(mouse_handler.fov), float(w) / float(h), 0.01f, 1000.0f);
    //glm::mat4 mat_P = glm::ortho(0.0f, float(w), 0.0f, float(h), 0.01f, 100.0f);
    
    // world to camera
    glm::mat4 mat_V;

    // model to world
    std::vector<glm::mat4> model_matrices;
    int N_cubes = 100;
    for (int i = 0; i <= N_cubes; ++i)
    {
        for (int j = 0; j <= N_cubes; ++j)
        {
            glm::vec3 t_vector(i-N_cubes/2, -1.0f, j-N_cubes/2);
            model_matrices.push_back(glm::translate(glm::mat4(1.0f), t_vector) * glm::scale(glm::mat4(1.0f), { 0.5, 0.5, 0.5 }));
            //model_matrices.push_back(glm::scale(glm::mat4(1.0f), { 0.5, 0.5, 0.5 }) * glm::translate(glm::mat4(1.0f), t_vector));
        }
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    double t = glfwGetTime(), dt;

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

        processInput(window, zqsd, mouse_handler.fov);
        mouse_handler.update(dt);
        mouse_handler.print(std::cout);
        {
            const float cam_speed = 3.f;
            camera.move(zqsd * float(dt) * cam_speed);
        }
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        camera.setDirection(mouse_handler.direction<float>());
        mat_V = camera.getMatrix();

        glBindVertexArray(VAO);
        program.use();
        program.setUniform("u_V", mat_V);
        program.setUniform("u_P", mat_P);
        program.setUniform("u_color", glm::vec3(1.0f, 0.0f, 1.0f));
        
        for (glm::mat4& mat_M : model_matrices)
        {
            program.setUniform("u_M", glm::rotate(mat_M, glm::radians(float(t)), {1.0f, 0.0f, 0.0f}));
            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
        lib::ProgramDesc::useNone();
    }



    glfwTerminate();
    return main_res;
}

