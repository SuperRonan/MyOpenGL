#include <cassert>
#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <type_traits>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <lib/ShaderDesc.h>
#include <lib/ProgramDesc.h>
#include <lib/Vertex.h>
#include <lib/Log.h>
#include <lib/Camera.h>
#include <lib/MouseHandler.h>
#include <lib/Mesh.h>
#include <lib/Scene.h>
#include <lib/Shapes.h>
#include <lib/Transforms.h>
#include <lib/Library.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
    using Node = lib::Node<float>;
    using Drawable = lib::Drawable<float>;
    using ProgramLibrary = lib::Library<lib::ProgramDesc>;
    using MaterialLibrary = lib::Library<lib::Material>;

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

    MaterialLibrary mat_lib;
    ProgramLibrary prog_lib;
    
    Scene scene;
    {
        std::shared_ptr<lib::Material> phong = mat_lib.addDerived("phong", lib::Phong<float>(lib::Vector3f{ 0.8f, 0.4f, 0.1f }, lib::Vector4f{ 1.f, 1.f, 1.f, 100.f }));
        std::shared_ptr<lib::Material> emissive = mat_lib.addDerived("emissive", lib::Phong<float>(lib::Vector3f(0), lib::Vector4f(0, 0, 0, 1), lib::Vector3f(1)));
        std::shared_ptr<lib::ProgramDesc> normal_prog = prog_lib.addBase("normal_viewer", lib::ProgramDesc(lib::Material::shaderPath().string() + "vector", true));
        std::shared_ptr<lib::Material> normal_mat = mat_lib.addBase("normal_viewer", normal_prog);

        lib::Shape<float, GLuint> cylinder = cylinder.Cylinder(1.0, 1.0, 20, true, true);
        lib::Shape<float, GLuint> cube_shape = cube_shape.Cube(true);

        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(cylinder);
        std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(cube_shape);

        mesh->setup();
        mesh->clearHost();

        cube->setup();
        cube->clearHost();

        Node* base = &scene.base;



        Drawable obj = { mesh, phong };
        base->sons.push_back(std::make_shared<Node>(lib::translateMatrix<4, float>({ 0, -1, 0 }) * lib::scaleMatrix<4, float>({ 2, 0.1, 2 })));
        base->sons[0]->addDrawable(std::move(obj));

        std::shared_ptr<Node> cube_node = std::make_shared<Node>(lib::scaleMatrix<4, float>(0.2));
        cube_node->emplaceDrawable(cube, emissive);

        Node node1 = lib::translateMatrix<4, float>(Vector3{ 2.f, 1.f, 3.f });
        Node node2 = lib::translateMatrix<4, float>(Vector3{ 0.f, 1.f, -3.f });

        node1.sons.push_back(cube_node);
        node2.sons.push_back(cube_node);

        base->sons.push_back(std::make_shared<Scene::Node>(std::move(node1)));
        base->sons.push_back(std::make_shared<Scene::Node>(std::move(node2)));


        scene.m_lights[0] = lib::Light<float>::PointLight({ 2.f, 1.f, 3.f }, Vector3(10));
        scene.m_lights[1] = lib::Light<float>::PointLight({ 0.f, 1.f, -3.f }, Vector3(10));

        scene.m_ambiant = { 0.3, 0.3, 0.3 };

        

        //scene.m_camera.setPosition(scene.m_lights[0].position);
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
        //mouse_handler.print(std::cout);
        {
            const float cam_speed = 3.f;
            scene.m_camera.move(zqsd * float(dt) * cam_speed);
        }
        glClearColor(scene.m_ambiant[0], scene.m_ambiant[1], scene.m_ambiant[2], 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.update(t, dt);
        
        scene.m_camera.setDirection(mouse_handler.direction<float>());

        scene.draw();
        scene.customDraw(mat_lib["normal_viewer"].get());

        lib::ProgramDesc::useNone();
    }



    glfwTerminate();
    return main_res;
}

