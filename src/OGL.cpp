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
#include <lib/Mesh.h>
#include <lib/Scene.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/// <summary>
/// Fills the vertices and indices arrays to make a cube, centered on 0, of size 1
/// </summary>
template <class Float>
void makeCube(std::vector<lib::Vertex<Float>>& vertices, std::vector<GLuint>& indices, bool use_s_normals=false)
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

    const Float h = Float(0.5);
    const Float t = Float(1) / Float(3);
    const Float tt = Float(2) / Float(3);

    

    if (use_s_normals)
    {
        const auto makeVertex = [](Float x, Float y, Float z, Float u, Float v)
        {return lib::Vertex<Float>({ x, y, z }, { x, y, z }, { u, v }); };

        // half
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
    else
    {
        lib::Vector3<Float> X(1, 0, 0), Y(0, 1, 0), Z(0, 0, 1);
        const auto sign = [](Float f) {return f > 0 ? Float(1) : (f < 0 ? Float(-1) : Float(0)); };
        const auto addVertexes = [&](Float x, Float y, Float z, Float u, Float v)
        {
            vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(x) * X, lib::Vector2<Float>{u, v});
            vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(y) * Y, lib::Vector2<Float>{u, v});
            vertices.emplace_back(lib::Vector3<Float>{x, y, z}, sign(z) * Z, lib::Vector2<Float>{u, v});
        };
        vertices.clear();

        addVertexes(-h, h, -h, 0, 0); // x0
        addVertexes(h, h, -h, h, 0); // x1
        addVertexes(h, h, h, h, h); // x2
        addVertexes(-h, h, h, 0, h); // x3
        addVertexes(-h, -h, -h, h, 0); // x4
        addVertexes(h, -h, -h, 1, h); // x5
        addVertexes(h, -h, h, 1, 1); // x6
        addVertexes(-h, -h, h, h, h); // x7

        const auto id = [](GLuint xi, GLuint axis) {return xi * 3 + axis; };
        
        const auto addFace = [&](GLuint v0, GLuint v1, GLuint v2, GLuint v3, GLuint axis)
        {
            indices.push_back(id(v0, axis));
            indices.push_back(id(v1, axis));
            indices.push_back(id(v2, axis));

            indices.push_back(id(v2, axis));
            indices.push_back(id(v3, axis));
            indices.push_back(id(v0, axis));
        };
        addFace(0, 3, 2, 1, 1); // up (+y)
        addFace(0, 1, 5, 4, 2); // back (-z)
        addFace(0, 4, 7, 3, 0); // left (-x)
        addFace(6, 7, 4, 5, 1); // bottom (-y)
        addFace(6, 2, 3, 7, 2); // front (+z)
        addFace(6, 5, 1, 2, 0); // right (+x)
    }
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
    using Mesh = lib::Mesh<float>;
    using Scene = lib::Scene<float>;

    using Matrix4 = lib::Matrix4f;
    using Vector3 = lib::Vector3f;

    int main_res = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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


    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    makeCube(vertices, indices, false);

    Mesh cube;

    cube.set(vertices, indices);
    cube.setup();

    Scene scene;

    Scene::Object obj = { std::make_shared<Mesh>(std::move(cube)), 
        std::make_shared <lib::Phong<float>>(lib::Vector3f{1.f, 0.f, 0.f}, lib::Vector4f{1.f, 1.f, 1.f, 100.f}) };
    scene.base.m_objects.push_back(std::make_shared<Scene::Object>(std::move(obj)));


    scene.m_lights[0] = lib::Light<float>::PointLight({2.f, 1.f, 3.f}, Vector3(10));
    scene.m_lights[1] = lib::Light<float>::PointLight({0.f, 1.f, -3.f}, Vector3(5));

    scene.m_camera.setPosition(scene.m_lights[0].position);
    
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
        //mouse_handler.print(std::cout);
        {
            const float cam_speed = 3.f;
            scene.m_camera.move(zqsd * float(dt) * cam_speed);
        }
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        scene.m_camera.setDirection(mouse_handler.direction<float>());

        scene.draw();

        lib::ProgramDesc::useNone();
    }



    glfwTerminate();
    return main_res;
}

