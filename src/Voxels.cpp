#include <cassert>
#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <type_traits>
#include <numeric>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <img/Image.h>
#include <img/Color.h>
#include <img/ImRead.h>


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

#include <voxels/World.h>

void processInput(GLFWwindow* window, glm::vec3& moving, float& fov, float& speed)
{
	speed = 1;
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 4;
}

GLFWwindow* createCenteredWindow(int w, int h, const char* name)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	GLFWwindow* res = glfwCreateWindow(w, h, name, NULL, NULL);
	glfwSetWindowPos(res, (mode->width - w) / 2, (mode->height - h) / 2);
	return res;
}

void reverseNormal(img::Image<img::io::RGBu>& normal_map, bool x_axis = true, bool y_axis = true)
{
	using RGBu = img::io::RGBu;
	normal_map.loop1D([&](int id)
		{
			RGBu& pixel = normal_map[id];
			if (x_axis)
				pixel[0] = 255 - pixel[0];
			if (y_axis)
				pixel[1] = 255 - pixel[1];
		});
}


void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

struct Property
{
	int32_t flag;
	int32_t face_ids[6];
	int32_t _pad;
};



GLenum CHECK_GL_ERROR()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "GL ERROR! " << error << std::endl;
	}
	return error;
}

std::vector<img::Image<img::io::RGBAu>> parseAtlas(img::Image<img::io::RGBAu> const& atlas, int resolution=16)
{
	int N = atlas.width() / resolution;
	std::vector<img::Image<img::io::RGBAu>> res(N);
	for (int i = 0; i < N; ++i)
	{
		res[i] = img::Image<img::io::RGBAu>(resolution, resolution);
		for (int y = 0; y < resolution; ++y)
		{
			for (int x = 0; x < resolution; ++x)
			{
				res[i](x, y) = atlas(i * resolution + x, y);
			}
		}
	}
	return res;
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

	lib::Window window(w, h, "Voxels go Brrrrrr...");
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

	CHECK_GL_ERROR();
	
	{
		img::Image<img::io::RGBAu> terrain_atlas = img::io::read<img::io::RGBu>("../ressources/voxels/terrain.png");
		std::vector< img::Image<img::io::RGBAu>> atlas = parseAtlas(terrain_atlas);

		GLuint atlas_handle;
		glGenTextures(1, &atlas_handle);
		glBindTexture(GL_TEXTURE_2D_ARRAY, atlas_handle);
		CHECK_GL_ERROR();
		
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, atlas[0].width(), atlas[0].height(), atlas.size());
		CHECK_GL_ERROR();
		for (int i = 0; i < atlas.size(); ++i)
		{
			glTexSubImage3D(
				GL_TEXTURE_2D_ARRAY, 
				0, // mip map id
				0, 0, i, // layer i
				atlas[i].width(), atlas[i].height(), // layer size
				1, // 1 layer at the time
				GL_RGBA,
				GL_UNSIGNED_BYTE, 
				atlas[i].data()
			);
			CHECK_GL_ERROR();
		}

		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		CHECK_GL_ERROR();


		double t = glfwGetTime(), dt;

		Camera camera;

		World world;

		CHECK_GL_ERROR();

		int32_t OPAQUE = 1;
		int32_t NON_OPAQUE = 0;

		std::vector<Property> properties(256);
		
		properties[0] = Property{ NON_OPAQUE, { 0, 0, 0, 0, 0, 0 }}; // Air or debug
		properties[1] = Property{OPAQUE, { 1, 1, 1, 1, 1, 1 }}; // Stone
		properties[2] = Property{OPAQUE, { 4, 4, 4, 4, 4, 4 }}; // Dirt
		properties[3] = Property{OPAQUE, { 3, 3, 2, 4, 3, 3 }}; // Grass Dirt

		GLuint props_handle;
		glGenBuffers(1, &props_handle);
		glBindBuffer(GL_UNIFORM_BUFFER, props_handle);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(Property) * properties.size(), properties.data(), GL_STATIC_READ);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		CHECK_GL_ERROR();

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
			float speed;
			processInput(window.get(), zqsd, mouse_handler.fov, speed);
			mouse_handler.update(dt);
			//mouse_handler.print(std::cout);
			{
				const float cam_speed = 10.f * 5 * speed;
				camera.move(zqsd * float(dt) * cam_speed);
			}
			camera.setDirection(mouse_handler.direction<float>());

			world.update(camera.getPosition());
			world.buildDrawList(camera);
			
			glClearColor(0.5, 0.5, 1.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glBindBuffer(GL_UNIFORM_BUFFER, props_handle);
			glBindBufferBase(GL_UNIFORM_BUFFER, 10, props_handle);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, atlas_handle);

			world.draw(camera);
			
			lib::ProgramDesc::useNone();
		}

	}




	



	glfwTerminate();
	return main_res;
}

