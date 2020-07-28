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
#include <lib/Shapes.h>
#include <lib/Transforms.h>

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


    std::shared_ptr<lib::Material> phong = 
        std::make_shared<lib::Phong<float>>(lib::Vector3f{ 0.8f, 0.4f, 0.1f }, lib::Vector4f{ 1.f, 1.f, 1.f, 100.f });
    std::shared_ptr<lib::Material> emissive =
        std::make_shared<lib::Phong<float>>(lib::Vector3f{ 0.f, 0.f, 0.f });

    
    lib::Shape<float, GLuint> cylinder = cylinder.Cylinder(1.0, 1.0, 20, true, true);
    lib::Shape<float, GLuint> cube_shape = cube_shape.Cube();

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(cylinder);
    std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(cube_shape);
    
    mesh->setup();
    mesh->clearHost();

    cube->setup();
    cube->clearHost();

    Scene scene;

    Scene::Object obj = { mesh, phong};
    scene.base.m_objects.push_back(std::make_shared<Scene::Object>(std::move(obj)));

    Scene::Node node1 = lib::translateMatrix<4, float>(Vector3{ 2.f, 1.f, 3.f }) * lib::scaleMatrix<4, float>(0.2);
    Scene::Node node2 = lib::translateMatrix<4, float>(Vector3{ 0.f, 1.f, -3.f }) * lib::scaleMatrix<4, float>(0.2);

    node1.m_objects.push_back(std::make_shared<Scene::Object>(cube, emissive));
    node2.m_objects.push_back(std::make_shared<Scene::Object>(cube, emissive));

    scene.base.m_sons.push_back(std::make_shared<Scene::Node>(std::move(node1)));
    scene.base.m_sons.push_back(std::make_shared<Scene::Node>(std::move(node2)));


    scene.m_lights[0] = lib::Light<float>::PointLight({2.f, 1.f, 3.f}, Vector3(10));
    scene.m_lights[1] = lib::Light<float>::PointLight({0.f, 1.f, -3.f}, Vector3(5));


    //scene.m_camera.setPosition(scene.m_lights[0].position);
    
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

