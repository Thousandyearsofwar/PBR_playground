#include "stb_image.h"
#include "Includes\glad\glad.h"
#include "Includes\glfw3.h"

#include"Shader.h"
#include"Camera.h"
#include"Scene.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include<vector>
using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);




const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


//Camera attribute
Camera camera(glm::vec3(0.0f, 0.5f, 3.0f));
float deltatime = 0.0f;
float lastFrame = 0.0f;

//mouse input
bool firstMouse = true;

int main()
{
	//Windows create
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PBRTPlayground", NULL, NULL);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (window == NULL)
	{
		std::cout << "Fail to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Callback Function setting
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Fail to initialize GLAD" << std::endl;
		return -1;
	}


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	Scene::init(SCR_WIDTH, SCR_HEIGHT);
	//Shaders
	Scene::LoadShaderAndTexture();

	//FBO
	Scene::configDeferredShadingFrameBuffer();
	Scene::configDepthCubeMapFBO_Point();
	Scene::congfigHDRFrameBuffer();
	Scene::configPingPongBlurFrameBuffer();

	


	//VAO VBO
	Scene::configPlane0();
	Scene::configPlane1();
	Scene::configQuad();
	Scene::configNaNoSuitModel();
	Scene::configSphere();
	Scene::configCube();
	Scene::configSkyBox();

	Scene::configLight();

	Scene::renderDepthMap_Point();

	Scene::equiretangularToCubeMapPass();
	Scene::irradiancceMapPass();
	Scene::PrefilterMapPass();
	Scene::LUTTextureGeneratePass();


	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//render loop
	while (!glfwWindowShouldClose(window))
	{

		float timeValue = glfwGetTime();
		float currentFrame = timeValue;
		deltatime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//input
		processInput(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene::Debug();


		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	//glDeleteVertexArrays(1, &Scene::quadVAO);
	//glDeleteBuffers(1, &Scene::quadVBO);

	//glfw: terminate,clear all GLFW resources
	glfwTerminate();

	return 0;
}


//glfw window input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(RIGHT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(SPACE, deltatime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		Scene::cameraProcessKeyboard(CONTROL, deltatime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
	glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);
	if (firstMouse)
	{
		firstMouse = false;
	}
	float xoffset = xpos - SCR_WIDTH / 2;
	float yoffset = SCR_HEIGHT / 2 - ypos;
	Scene::cameraProcessMouseMovement(xoffset, yoffset);
	glfwSetCursorPos(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	Scene::cameraProcessMouseScroll(yoffset);
}
//glfw window  size changed callback function
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}