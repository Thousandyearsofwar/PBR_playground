#pragma once
#include "Includes\glad\glad.h"
#include "Includes\glfw3.h"
#include"ResourceManager.h"

#include"Camera.h"
#include"Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define PI 3.14159265359

class Scene
{
private:
	Scene();
	~Scene();
	static Camera camera;
	static int SCR_WIDTH ;
	static int SCR_HEIGHT ;
public:
	static void init(int scr_width,int scr_height);

	static void cameraProcessMouseMovement(double xoffset, double yoffset);
	static void cameraProcessMouseScroll(double yoffset);
	static void cameraProcessKeyboard(Camera_Movement direction, float deltaTime);

	static void LoadShaderAndTexture();

	static void configPlane0();
	static void configPlane1();
	static void configQuad();
	static void configSphere();
	static void configCube();

	static void configNaNoSuitModel();
	static void configSkyBox();


	static void configDeferredShadingFrameBuffer();
	static void congfigHDRFrameBuffer();
	static void configPingPongBlurFrameBuffer();
	static void configDepthCubeMapFBO_Point();





	static void configLight();

	static void renderPlane0();
	static void renderPlane1();
	static void renderQuad();
	static void renderSphere();
	static void renderCube();




	static void renderDepthMap_Point();
	static void equiretangularToCubeMapPass();
	static void irradiancceMapPass();
	static void LUTTextureGeneratePass();
	static void PrefilterMapPass();


	static void renderScene_Point(Shader &shader);
	static void renderScene_LightSphere(Shader &shader);
	static void renderScene_SkyBox(Shader &shader);

	static void finalRender_Point();

	static void Debug();



};

