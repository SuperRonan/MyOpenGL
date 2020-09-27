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

#include <img/Image.h>
#include <img/Color.h>
#include <img/ImRead.h>

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
#include <lib/StreamOperators.h>
#include <lib/Drawable.h>
#include <lib/Window.h>
#include <lib/Texture.h>

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

void reverseNormal(img::Image<img::io::RGBu>& normal_map, bool x_axis=true, bool y_axis=true)
{
    using RGBu = img::io::RGBu;
    normal_map.loop1D([&](int id)
        {
            RGBu& pixel = normal_map[id];
            if(x_axis)
                pixel[0] = 255 - pixel[0];
            if(y_axis)
                pixel[1] = 255 - pixel[1];
        });
}



int main()
{
    using Vertex = lib::Vertex<float>;
    using Camera = lib::Camera<float>;
    using Mesh = lib::Mesh<float>;
    using Scene = lib::Scene<float>;
    using Node = lib::Node<float>;
    using LambdaNode = lib::LambdaNode<float>;
    using Drawable = lib::Drawable<float>;
    using Light = lib::Light<float>;
    using ProgramLibrary = lib::Library<lib::ProgramDesc>;
    using MaterialLibrary = lib::Library<lib::Material>;
    using TextureLibrary = lib::Library<lib::Texture>;

    using Matrix4 = lib::Matrix4f;
    using Vector3 = lib::Vector3f;

    int main_res = 0;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int w = 1920, h = 1080;
    
    lib::Window window(w, h, "OpenGL go Brrrrrr...");
    if (!window.isOk())
    {
        glfwTerminate();
        return -1;
    }
    window.makeCurrent();
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Could not initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    lib::MouseHandler mouse_handler(window.get());

    MaterialLibrary mat_lib;
    ProgramLibrary prog_lib;
    TextureLibrary tex_lib;

    Scene scene;
    {
        img::Image<img::io::RGBu> img_diffuse = img::io::read<img::io::RGBu>("../ressources/brickwall.png");
        img::Image<img::io::RGBu> img_normal = img::io::read<img::io::RGBu>("../ressources/brickwall_normal.png");
        reverseNormal(img_normal, false, true);

        std::shared_ptr<lib::Texture> texture_diffuse = std::make_shared<lib::_Texture<img::io::RGBu>>(img_diffuse);
        std::shared_ptr<lib::Texture> texture_normal = std::make_shared<lib::_Texture<img::io::RGBu>>(img_normal);
        
        texture_diffuse->sendToDevice();
        texture_normal->sendToDevice();
        texture_diffuse->deleteHost();
        texture_normal->deleteHost();

        tex_lib.add("diffuse", texture_diffuse);
        tex_lib.add("normal", texture_normal);

        std::shared_ptr<lib::Phong<float>> phong = std::make_shared<lib::Phong<float>>(
            lib::Phong<float>(lib::Vector3f{ 1, 1, 1 }, lib::Vector4f{ 1.f, 1.f, 1.f, 100.f }));
        phong->m_diffuse_tex = texture_diffuse;
        phong->m_normal_map = texture_normal;
        mat_lib.add("phong", phong);

        std::shared_ptr<lib::Material> emissive = mat_lib.addDerived("emissive", lib::Phong<float>(lib::Vector3f(0), lib::Vector4f(0, 0, 0, 1), lib::Vector3f(1)));
        std::shared_ptr<lib::ProgramDesc> vector_prog = prog_lib.addBase("vector_viewer", lib::ProgramDesc(lib::Material::shaderPath().string() + "vector", true));
        std::shared_ptr<lib::Material> vector_mat = mat_lib.addBase("vector_viewer", vector_prog);
        vector_prog->setUniform("u_t_normal", 0);

        lib::Shape<float, GLuint> cylinder = cylinder.Cylinder(1.0, 1.0, 200, true, true);
        lib::Shape<float, GLuint> cube_shape = cube_shape.Cube();

        std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(cylinder);
        std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(cube_shape);

        mesh->setup();
        mesh->clearHost();

        cube->setup();
        cube->clearHost();

        Node* base = &scene.base;



        Drawable obj = { mesh, phong };
        //base->sons.push_back(std::make_shared<Node>(lib::translateMatrix<4, float>({ 0, -1, 0 }) * lib::scaleMatrix<4, float>({ 1.41, 3, 1.41 })));
        LambdaNode cylinder_rotate = LambdaNode([](float t, float dt, Matrix4& mat)
            {
                //mat *= glm::rotate(Matrix4(1), glm::radians(dt * 10), Vector3(0, 1, 0));
            });
        cylinder_rotate.addNew<Drawable>(std::move(obj));
        base->sons.push_back(std::make_shared<LambdaNode>(std::move(cylinder_rotate)));

        std::shared_ptr<Node> cube_node = std::make_shared<Node>(lib::scaleMatrix<4, float>(0.1));
        cube_node->emplaceNew<Drawable>(cube, emissive);
        cube_node->emplaceNew<Light>(Light::PointLight(Vector3(0), Vector3(5)));
        
        Node node1 = lib::translateMatrix<4, float>(Vector3{ 2.f, 1.5f, 1.f });
        LambdaNode rotate = LambdaNode([](float t, float dt, Matrix4& mat)
            {
                mat *= glm::rotate(Matrix4(1), glm::radians(dt*50), Vector3(0, 1, 0));
            });
        Node node2 = lib::translateMatrix<4, float>(Vector3{ 0.f, 2.f, 1.f });

        node1.sons.push_back(cube_node);
        node2.sons.push_back(cube_node);

        rotate.sons.push_back(std::make_shared<Node>(std::move(node2)));

        base->sons.push_back(std::make_shared<Node>(std::move(node1)));
        base->sons.push_back(std::make_shared<LambdaNode>(std::move(rotate)));

        scene.base.emplaceNew<Light>(Light::DirectionalLight(glm::normalize(-Vector3(1, 1, 1)), Vector3(0.5)));

        scene.m_ambiant = { 0.3, 0.3, 0.3 };
        
        //scene.m_camera.setPosition(scene.m_lights[0].position);
    }

    
    double t = glfwGetTime(), dt;
    

    while (!window.shouldClose())
    {
        {
            double new_t = glfwGetTime();
            dt = new_t - t;
            t = new_t;
        }
        window.swapBuffers();
        glfwPollEvents();

        glm::vec3 zqsd;

        processInput(window.get(), zqsd, mouse_handler.fov);
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
        //tex_lib["normal"]->use(0);
        //scene.customDraw(mat_lib["vector_viewer"].get());

        lib::ProgramDesc::useNone();
    }



    glfwTerminate();
    return main_res;
}

