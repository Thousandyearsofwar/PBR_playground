#include "Scene.h"

//VAO VBO
static GLuint plane0VAO, plane0VBO;
static GLuint plane1VAO, plane1VBO;

static GLuint SkyBoxVAO, SkyBoxVBO;
static GLuint CubeVAO, CubeVBO;

static GLuint quadVAO;
static GLuint quadVBO;

static GLuint sphereVAO,sphereVBO,sphereEBO;
static unsigned int indexCount;


//FBO
static GLuint depthMapFBO_Point;

static GLuint gBuffer;

static GLuint hdrFBO;

static GLuint pingpongFBO[2];

//Generate
static GLuint equirectangularMap2CubeFBO;
static GLuint irradianceMapFBO;
static GLuint prefilterMapFBO;
static GLuint brdfLUTtextureFBO;


static GLuint captureRBO;


//


Camera Scene::camera(glm::vec3(5.0f, 5.0f, 0.0f));
int nrRows = 7;
int nrColumns = 7;
float spacing = 2.5;

static GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
static GLuint LUTTexture_Width, LUTTexture_HEIGHT;

static GLfloat near_plane, far_plane;

int Scene::SCR_WIDTH;
int Scene::SCR_HEIGHT;

static glm::vec3 LightPos = glm::vec3(2.0f, -4.0f, 0.0f);
static glm::vec3 LightColor = glm::vec3(3.0f);

static glm::vec3 PbrtLightPos[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
};
static glm::vec3 PbrtLightColor[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
};



vector<std::string> faces =
{
	"Resources/skybox/right.jpg",
	"Resources/skybox/left.jpg",
	"Resources/skybox/top.jpg",
	"Resources/skybox/bottom.jpg",
	"Resources/skybox/front.jpg",
	"Resources/skybox/back.jpg"
};

glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
glm::mat4 captureViews[] =
{
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};


static GLfloat exposure=0.8f;


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::init(int scr_width, int scr_height) {
	Scene::SCR_WIDTH = scr_width;
	Scene::SCR_HEIGHT = scr_height;
}

void Scene::cameraProcessMouseMovement(double xoffset, double yoffset) {
	camera.ProcessMouseMovement(xoffset,yoffset);
}
void Scene::cameraProcessMouseScroll(double yoffset){
	camera.ProcessMouseScroll(yoffset);
}
void Scene::cameraProcessKeyboard(Camera_Movement direction, float deltaTime) {
	camera.ProcessKeyboard(direction,deltaTime);
}


//VAO
void Scene::configPlane0()
{
	glm::vec3 pos1(-1.0f,1.0f,0.0f);
	glm::vec3 pos2(-1.0,-1.0f,0.0f);
	glm::vec3 pos3(6.0f,-1.0f,0.0f);
	glm::vec3 pos4(6.0f,1.0f,0.0f);
	
	glm::vec2 uv1(0.0f,1.0f);
	glm::vec2 uv2(0.0f,0.0f);
	glm::vec2 uv3(6.0f,0.0f);
	glm::vec2 uv4(6.0f,1.0f);

	glm::vec3 nm(0.0f,0.0f,1.0f);

	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;

	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);
	tangent1.x = f * deltaUV2.y*edge1.x - deltaUV1.y*edge2.x;
	tangent1.y = f * deltaUV2.y*edge1.y - deltaUV1.y*edge2.y;
	tangent1.z = f * deltaUV2.y*edge1.z - deltaUV1.y*edge2.z;
	tangent1 = glm::normalize(tangent1);


	bitangent1.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
	bitangent1.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
	bitangent1.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
	bitangent1 = glm::normalize(bitangent1);



	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;
	f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);

	tangent2.x = f * deltaUV2.y*edge1.x - deltaUV1.y*edge2.x;
	tangent2.y = f * deltaUV2.y*edge1.y - deltaUV1.y*edge2.y;
	tangent2.z = f * deltaUV2.y*edge1.z - deltaUV1.y*edge2.z;
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
	bitangent2.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
	bitangent2.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
	bitangent2 = glm::normalize(bitangent2);





	GLfloat planeVertices[] = {
		//vec3				 //vec3		   //vec2		//vec3							//vec3
		// positions		 // Normals	   // texCoord  //tangent						//bitangent
		pos1.x,pos1.y,pos1.z,nm.x,nm.y,nm.z,uv1.x,uv1.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,
		pos2.x,pos2.y,pos2.z,nm.x,nm.y,nm.z,uv2.x,uv2.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,
		pos3.x,pos3.y,pos3.z,nm.x,nm.y,nm.z,uv3.x,uv3.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,

		pos1.x,pos1.y,pos1.z,nm.x,nm.y,nm.z,uv1.x,uv1.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z,
		pos3.x,pos3.y,pos3.z,nm.x,nm.y,nm.z,uv3.x,uv3.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z,
		pos4.x,pos4.y,pos4.z,nm.x,nm.y,nm.z,uv4.x,uv4.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z
	};

	glGenVertexArrays(1,&plane0VAO);

	glGenBuffers(1,&plane0VBO);

	glBindVertexArray(plane0VAO);
	glBindBuffer(GL_ARRAY_BUFFER,plane0VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(planeVertices),planeVertices,GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14* sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(11 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void Scene::configPlane1()
{
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);

	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);

	glm::vec3 nm(0.0f, 0.0f, 1.0f);

	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;

	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);
	tangent1.x = f * deltaUV2.y*edge1.x - deltaUV1.y*edge2.x;
	tangent1.y = f * deltaUV2.y*edge1.y - deltaUV1.y*edge2.y;
	tangent1.z = f * deltaUV2.y*edge1.z - deltaUV1.y*edge2.z;
	tangent1 = glm::normalize(tangent1);


	bitangent1.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
	bitangent1.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
	bitangent1.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
	bitangent1 = glm::normalize(bitangent1);



	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;
	f = 1.0f / (deltaUV1.x*deltaUV2.y - deltaUV2.x*deltaUV1.y);

	tangent2.x = f * deltaUV2.y*edge1.x - deltaUV1.y*edge2.x;
	tangent2.y = f * deltaUV2.y*edge1.y - deltaUV1.y*edge2.y;
	tangent2.z = f * deltaUV2.y*edge1.z - deltaUV1.y*edge2.z;
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x*edge1.x + deltaUV1.x*edge2.x);
	bitangent2.y = f * (-deltaUV2.x*edge1.y + deltaUV1.x*edge2.y);
	bitangent2.z = f * (-deltaUV2.x*edge1.z + deltaUV1.x*edge2.z);
	bitangent2 = glm::normalize(bitangent2);





	GLfloat planeVertices[] = {
		//vec3				 //vec3		   //vec2		//vec3							//vec3
		// positions		 // Normals	   // texCoord  //tangent						//bitangent
		pos1.x,pos1.y,pos1.z,nm.x,nm.y,nm.z,uv1.x,uv1.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,
		pos2.x,pos2.y,pos2.z,nm.x,nm.y,nm.z,uv2.x,uv2.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,
		pos3.x,pos3.y,pos3.z,nm.x,nm.y,nm.z,uv3.x,uv3.y,tangent1.x,tangent1.y,tangent1.z,bitangent1.x,bitangent1.y,bitangent1.z,

		pos1.x,pos1.y,pos1.z,nm.x,nm.y,nm.z,uv1.x,uv1.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z,
		pos3.x,pos3.y,pos3.z,nm.x,nm.y,nm.z,uv3.x,uv3.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z,
		pos4.x,pos4.y,pos4.z,nm.x,nm.y,nm.z,uv4.x,uv4.y,tangent2.x,tangent2.y,tangent2.z,bitangent2.x,bitangent2.y,bitangent2.z
	};

	glGenVertexArrays(1, &plane1VAO);

	glGenBuffers(1, &plane1VBO);

	glBindVertexArray(plane1VAO);
	glBindBuffer(GL_ARRAY_BUFFER, plane1VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (void*)(11 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void Scene::configQuad()
{
	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void Scene::configSkyBox()
{
	float vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &SkyBoxVAO);
	glGenBuffers(1, &SkyBoxVBO);

	glBindVertexArray(SkyBoxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//												步长值				偏移量

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Scene::configNaNoSuitModel()
{
	Model NaNoSuit = ResourceManager::LoadModel("Resources/nanosuit/nanosuit.obj", false, "NaNoSuit");
	
}

void Scene::configSphere() {
	glGenVertexArrays(1,&sphereVAO);
	glGenBuffers(1,&sphereVBO);
	glGenBuffers(1,&sphereEBO);
	
	std::vector<glm::vec3>pos;
	std::vector<glm::vec2>uv;
	std::vector<glm::vec3>normal;
	std::vector<unsigned int>indices;
	

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;

	for(unsigned int y=0;y<=Y_SEGMENTS;y++)
		for (unsigned int x = 0; x <= X_SEGMENTS; x++) {
			float xSegment =(float) x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			
			GLfloat xPos = std::cos(xSegment*2.0f*PI)*std::sin(ySegment*PI);
			GLfloat yPos = std::cos(ySegment*PI);
			GLfloat zPos = std::sin(xSegment*2.0f*PI)*std::sin(ySegment*PI);

			pos.push_back(glm::vec3(xPos,yPos,zPos));
			uv.push_back(glm::vec2(xSegment,ySegment));
			normal.push_back(glm::vec3(xPos, yPos, zPos));
		}
	
	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
		
		if (!oddRow)//even rows :y==0,y==2;and so on
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
				indices.push_back(y*(X_SEGMENTS+1)+x);
				indices.push_back((y + 1)*(X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (unsigned int x = X_SEGMENTS; x >= 0; --x) {
				indices.push_back((y+1)*(X_SEGMENTS+1)+x);
				indices.push_back(y*(X_SEGMENTS+1)+x);
			}
		}
		oddRow != oddRow;
	}
	indexCount = indices.size();

	std::vector<float> data;
	for (std::size_t i = 0; i < pos.size(); ++i)
	{
		data.push_back(pos[i].x);
		data.push_back(pos[i].y);
		data.push_back(pos[i].z);
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}
		if (normal.size() > 0)
		{
			data.push_back(normal[i].x);
			data.push_back(normal[i].y);
			data.push_back(normal[i].z);
		}
	}
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	float stride = (3 + 2 + 3) * sizeof(float);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));

	/*
	glBindVertexArray(sphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec3) + uv.size() * sizeof(glm::vec2) + normal.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, pos.size() * sizeof(glm::vec3), pos.data());
	glBufferSubData(GL_ARRAY_BUFFER, 0, uv.size() * sizeof(glm::vec2), uv.data());
	glBufferSubData(GL_ARRAY_BUFFER, 0, normal.size() * sizeof(glm::vec3), normal.data());
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,sphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),&indices[0],GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)(pos.size() * 3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)(pos.size() * 3 * sizeof(GLfloat) +uv.size()* 2 * sizeof(GLfloat)));
	*/


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Scene::configCube()
{
	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
		// front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		// left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		// right face
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
		// bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		// top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left      
	};

	glGenVertexArrays(1, &CubeVAO);
	glGenBuffers(1, &CubeVBO);

	glBindVertexArray(CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
	//												步长值				偏移量

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


}


void Scene::LoadShaderAndTexture()
{
	Shader dMShaderPoint = ResourceManager::LoadShader("Shaders/depthMapGenerator_Point.vert", "Shaders/depthMapGenerator_Point.frag", "Shaders/depthMapGenerator_Point.geom","dMShaderPoint");

	Shader GeometryPassShader = ResourceManager::LoadShader("Shaders/GeometryPass.vert", "Shaders/GeometryPass.frag", nullptr, "GeometryPassShader");

	Shader BLShader_Point =ResourceManager::LoadShader("Shaders/BLShader_Point.vert", "Shaders/BLShader_Point.frag",nullptr,"BLShader_Point");

	Shader LightBoxShader =ResourceManager::LoadShader("Shaders/lightBox.vert", "Shaders/lightBox.frag",nullptr,"LightBoxShader");

	Shader HDR_Blur =ResourceManager::LoadShader("Shaders/HDR_Blur.vert", "Shaders/HDR_Blur.frag",nullptr,"HDR_Blur");

	Shader ShaderBloomFinal=ResourceManager::LoadShader("Shaders/ShaderBloomFinal.vert", "Shaders/ShaderBloomFinal.frag", nullptr, "ShaderBloomFinal");


	Shader PbrShader = ResourceManager::LoadShader("Shaders/pbr.vert", "Shaders/pbr.frag",nullptr,"PbrShader");
	Shader IrradianceShader = ResourceManager::LoadShader("Shaders/IrradianceShader.vert", "Shaders/IrradianceShader.frag", nullptr, "IrradianceShader");
	Shader equiretangularToCubeMapShader = ResourceManager::LoadShader("Shaders/equiretangularToCubeMap.vert", "Shaders/equiretangularToCubeMap.frag", nullptr, "equiretangularToCubeMapShader");
	
	Shader brdfLUTTextureGenShader = ResourceManager::LoadShader("Shaders/brdfLUTTextureGen.vert", "Shaders/brdfLUTTextureGen.frag", nullptr, "brdfLUTTextureGenShader");
	Shader prefilterShader = ResourceManager::LoadShader("Shaders/PrefilterMap.vert", "Shaders/PrefilterMap.frag", nullptr, "PrefilterMapShader");
	

	Shader SkyBoxShader = ResourceManager::LoadShader("Shaders/sky.vert","Shaders/sky.frag",nullptr,"SkyBoxShader");

	Shader DebugShader= ResourceManager::LoadShader("Shaders/Debug.vert", "Shaders/Debug.frag", nullptr, "DebugShader");


	ResourceManager::LoadTexture2D("Resources/wood_COLOR.png","wood", true);
	ResourceManager::LoadTexture2D("Resources/wood_NRM.png","wood_Normal", true);
	ResourceManager::LoadTexture2D("Resources/wood_SPEC.png","wood_Specular", true);
	ResourceManager::LoadTexture2D("Resources/wood_DISP.png","wood_Disp", true);

	ResourceManager::LoadTexture2D("Resources/newport_loft.hdr","hdr",false,true);



	ResourceManager::LoadTextureCube(faces,false,"skybox");



	GeometryPassShader.Use().SetInteger("texture_diffuse1", 0);
	GeometryPassShader.Use().SetInteger("texture_specular1", 1);
	GeometryPassShader.Use().SetInteger("texture_normal1", 2);
	GeometryPassShader.Use().SetInteger("texture_height1", 3);

	PbrShader.Use().SetInteger("irradianceMap",0);
	PbrShader.Use().SetInteger("prefilterMap",1);
	PbrShader.Use().SetInteger("brdfLUTTexture",2);
	PbrShader.Use().SetVector3f("albedo",glm::vec3(0.5f,0.0f,0.0f));
	PbrShader.Use().SetFloat("ao",1.0f);

	


	BLShader_Point.Use().SetInteger("gPosition", 0);
	BLShader_Point.Use().SetInteger("gNormal", 1);
	BLShader_Point.Use().SetInteger("gAlbedoSpec", 2);
	BLShader_Point.Use().SetInteger("gTangentViewDir", 3);
	BLShader_Point.Use().SetInteger("gTangentLightDir", 4);
	BLShader_Point.Use().SetInteger("shadowDepthMap", 5);


	

	HDR_Blur.Use().SetInteger("image", 0);

	ShaderBloomFinal.Use().SetInteger("scene",0);
	ShaderBloomFinal.Use().SetInteger("bloomBlur",1);


}


//FrameBuffer

void Scene::configDeferredShadingFrameBuffer()
{
	glGenFramebuffers(1,&gBuffer);
	
	Texture2D gPosition, gNormal, gAlbedoSpec,gTangentViewDir, gTangentLightDir;
	gPosition.GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	gNormal.GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	gAlbedoSpec.GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	gTangentViewDir.GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	gTangentLightDir.GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);


	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,gPosition.ID,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D, gNormal.ID,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D, gAlbedoSpec.ID,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT3,GL_TEXTURE_2D, gTangentViewDir.ID,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT4,GL_TEXTURE_2D, gTangentLightDir.ID,0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLuint attachments[5] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3,GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5,attachments);

	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ResourceManager::LoadTexture2D(gPosition,"gPosition");
	ResourceManager::LoadTexture2D(gNormal,"gNormal");
	ResourceManager::LoadTexture2D(gAlbedoSpec,"gAlbedoSpec");
	ResourceManager::LoadTexture2D(gTangentViewDir,"gTangentViewDir");
	ResourceManager::LoadTexture2D(gTangentLightDir,"gTangentLightDir");

}

void Scene::congfigHDRFrameBuffer()
{
	glGenFramebuffers(1,&hdrFBO);

	Texture2D HDR_ColorBuffer[2];
	HDR_ColorBuffer[0].GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	HDR_ColorBuffer[1].GenerateHDRMap(SCR_WIDTH,SCR_HEIGHT);
	ResourceManager::LoadTexture2D(HDR_ColorBuffer[0],"HDR_ColorBuffer0");
	ResourceManager::LoadTexture2D(HDR_ColorBuffer[1],"HDR_ColorBuffer1");

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDR_ColorBuffer[0].ID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, HDR_ColorBuffer[1].ID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

	GLuint attachments[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2,attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Scene::configPingPongBlurFrameBuffer()
{
	glGenFramebuffers(2, pingpongFBO);
	Texture2D pingpongColorBuffer[2];
	

	for (int i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		pingpongColorBuffer[i].GenerateHDRMap(SCR_WIDTH, SCR_HEIGHT);
		ResourceManager::LoadTexture2D(pingpongColorBuffer[i], "pingpongColorBuffer"+std::to_string(i));

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffer[i].ID, 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer "+std::to_string(i)+"not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::configDepthCubeMapFBO_Point()
{
	glGenFramebuffers(1, &depthMapFBO_Point);

	SHADOW_WIDTH = 2048;
	SHADOW_HEIGHT = 2048;

	TextureCube depthMap_Point(SHADOW_WIDTH,SHADOW_HEIGHT, GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT,
		GL_FLOAT,GL_CLAMP_TO_EDGE,GL_NEAREST);
	depthMap_Point.GenerateDepthCubeMap(SHADOW_WIDTH, SHADOW_HEIGHT);
	ResourceManager::LoadTextureCube(depthMap_Point, "depthMap_Point");

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Point);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap_Point.ID, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}





//Light
void Scene::configLight()
{
	Shader BLShader_Point = ResourceManager::GetShader("BLShader_Point");
	BLShader_Point.Use();
	BLShader_Point.SetVector3f("LightPos", LightPos);
	BLShader_Point.SetVector3f("LightColor", LightColor);


	Shader PbrtShader = ResourceManager::GetShader("PbrShader");
	PbrtShader.Use();
	for (unsigned int i = 0; i < 4; i++) {
		PbrtShader.SetVector3f(("lightPosition["+std::to_string(i)+"]").c_str(),PbrtLightPos[i]);
		PbrtShader.SetVector3f(("lightColors["+std::to_string(i)+"]").c_str(),PbrtLightColor[i]);
	
	}



}

void Scene::renderPlane0()
{
	glBindVertexArray(plane0VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene::renderPlane1()
{
	glBindVertexArray(plane1VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Scene::renderQuad()
{
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Scene::renderSphere() {
	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP,indexCount,GL_UNSIGNED_INT,0);
	glBindVertexArray(0);
}

void Scene::renderCube()
{
	glBindVertexArray(CubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void Scene::renderDepthMap_Point()
{
	glCullFace(GL_FRONT);

	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	near_plane = 0.02f, far_plane = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near_plane, far_plane);

	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	);
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	);
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))
	);
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))
	);
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	);
	shadowTransforms.push_back(
		shadowProj*glm::lookAt(LightPos, LightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	);



	Shader dMShaderPoint = ResourceManager::GetShader("dMShaderPoint");
	dMShaderPoint.Use();
	for (int i = 0; i < 6; i++)
		dMShaderPoint.SetMatrix4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);

	dMShaderPoint.SetFloat("far_plane", far_plane);
	dMShaderPoint.SetVector3f("lightPos", LightPos);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_Point);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//demo shadow used
	dMShaderPoint.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	dMShaderPoint.SetMatrix4("model", model);
	renderPlane1();


	//bottom
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	dMShaderPoint.SetMatrix4("model", model);
	renderPlane0();

	//right
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.5f, 1.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	dMShaderPoint.SetMatrix4("model", model);
	renderPlane0();

	//left
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.5f, -1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	dMShaderPoint.SetMatrix4("model", model);
	renderPlane0();

	//top
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	dMShaderPoint.SetMatrix4("model", model);
	renderPlane0();

	//front
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(6.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	dMShaderPoint.SetMatrix4("model", model);
	renderPlane1();


	//back
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	dMShaderPoint.SetMatrix4("model", model);
	renderPlane1();

	//NanoSuit
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.1f));
	dMShaderPoint.SetMatrix4("model", model);
	
	Model NaNoSuit = ResourceManager::GetModel("NaNoSuit");
	NaNoSuit.Draw(dMShaderPoint);

	glCullFace(GL_BACK);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

}

void Scene::equiretangularToCubeMapPass() {

	//Config------
	glGenFramebuffers(1, &equirectangularMap2CubeFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, equirectangularMap2CubeFBO);

	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, captureRBO);
	

	//Render------
	glViewport(0, 0, 512, 512);
	//Input Texture
	Texture2D equirectangularMapHDR = ResourceManager::GetTexture2D("hdr");

	//Pass Output Texture
	TextureCube envCubeMap(512, 512, GL_RGBA16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);
	envCubeMap.Generate(512, 512);
	ResourceManager::LoadTextureCube(envCubeMap, "envCubeMap");


	//Shader
	Shader equiretangularToCubeMapShader = ResourceManager::GetShader("equiretangularToCubeMapShader");
	equiretangularToCubeMapShader.Use();
	equiretangularToCubeMapShader.SetInteger("equirectangularMap", 0);
	equiretangularToCubeMapShader.SetMatrix4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	equirectangularMapHDR.Bind();

	for (unsigned int i = 0; i < 6; i++) {
		equiretangularToCubeMapShader.SetMatrix4("view",captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			envCubeMap.ID,0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		renderCube();
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::irradiancceMapPass(){
	
	//Config------
	glGenFramebuffers(1, &irradianceMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, irradianceMapFBO);
	
	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, captureRBO);
	

	//Render------
	glViewport(0,0,32,32);

	
	
	//Input Texture
	TextureCube environmentTextureCube = ResourceManager::GetTextureCube("envCubeMap");

	//Pass Output Texture
	TextureCube irradianceMap(32, 32, GL_RGBA16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);
	irradianceMap.Filter_MIN = GL_LINEAR_MIPMAP_LINEAR;
	irradianceMap.Generate(32, 32,true);
	ResourceManager::LoadTextureCube(irradianceMap, "irradianceMap");


	//Shader
	Shader irradiancePassShader = ResourceManager::GetShader("IrradianceShader");
	irradiancePassShader.Use();
	irradiancePassShader.SetInteger("EnvironmentMap",0);
	irradiancePassShader.SetMatrix4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	environmentTextureCube.Bind();
	
	for (unsigned int i = 0; i < 6; ++i) {
		irradiancePassShader.SetMatrix4("view",captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
			irradianceMap.ID,0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
		renderCube();
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

//need correct
void Scene::LUTTextureGeneratePass()
{
	//Config------
	glGenFramebuffers(1, &brdfLUTtextureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, brdfLUTtextureFBO);

	glGenRenderbuffers(1, &captureRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, captureRBO);

	//Render-------
	glViewport(0, 0, 512, 512);


	//Pass Output Texture

	Texture2D brdfLUTTexture(512,512,GL_RGBA,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,GL_LINEAR);
	brdfLUTTexture.Generate(512,512);
	ResourceManager::LoadTexture2D(brdfLUTTexture,"brdfLUTTexture");

	Shader brdfLUTTextureGenShader = ResourceManager::GetShader("brdfLUTTextureGenShader");
	brdfLUTTextureGenShader.Use();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture.ID, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;


	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);




}

void Scene::PrefilterMapPass()
{
	//Config------
	glGenFramebuffers(1, &prefilterMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, prefilterMapFBO);


	//Render-------
	
	//Input Texture


	//Pass Output Texture

	TextureCube prefilterMap(128, 128, GL_RGBA16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR);
	prefilterMap.Filter_MIN = GL_LINEAR_MIPMAP_LINEAR;
	prefilterMap.Generate(128,128,true);
	ResourceManager::LoadTextureCube(prefilterMap,"prefilterMap");

	
	//Shader
	Shader prefilterShader = ResourceManager::GetShader("PrefilterMapShader");
	prefilterShader.Use();
	prefilterShader.SetInteger("EnvironmentMap",0);
	prefilterShader.SetMatrix4("projection",captureProjection);
	TextureCube envCube = ResourceManager::GetTextureCube("envCubeMap");
	envCube.Bind();
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels;++mip) {
		unsigned int mipWidth = 128 * std::pow(0.5,mip);
		unsigned int mipHeight = 128 * std::pow(0.5,mip);


		glGenRenderbuffers(1, &captureRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_RENDERBUFFER, captureRBO);

		glViewport(0,0,mipWidth,mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels-1);
		prefilterShader.SetFloat("roughness",roughness);
		for (unsigned int i = 0; i < 6; ++i) {
			prefilterShader.SetMatrix4("view",captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,prefilterMap.ID,mip);
			
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Scene::finalRender_Point()
{
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glBindFramebuffer(GL_FRAMEBUFFER,gBuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	Shader GeometryPassShader = ResourceManager::GetShader("GeometryPassShader");


	Texture2D woodTexture = ResourceManager::GetTexture2D("wood");
	Texture2D woodNormalTexture = ResourceManager::GetTexture2D("wood_Normal");
	Texture2D woodSpecularTexture = ResourceManager::GetTexture2D("wood_Specular");
	Texture2D woodDisplaceTexture = ResourceManager::GetTexture2D("wood_Disp");


	

	GeometryPassShader.Use();
	
	GeometryPassShader.SetMatrix4("projection", projection);
	GeometryPassShader.SetMatrix4("view", view);

	GeometryPassShader.SetVector3f("viewPos", camera.Position);
	GeometryPassShader.SetVector3f("lightPos", LightPos);

	GeometryPassShader.SetFloat("far_plane", far_plane);

	glActiveTexture(GL_TEXTURE0);
	woodTexture.Bind();

	glActiveTexture(GL_TEXTURE1);
	woodSpecularTexture.Bind();

	glActiveTexture(GL_TEXTURE2);
	woodNormalTexture.Bind();


	renderScene_Point(GeometryPassShader);

	glBindFramebuffer(GL_FRAMEBUFFER,0);


	//Normal Render
	glBindFramebuffer(GL_FRAMEBUFFER,hdrFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	Shader BLShader_Point = ResourceManager::GetShader("BLShader_Point");


	TextureCube depthMap_Point = ResourceManager::GetTextureCube("depthMap_Point");

	BLShader_Point.Use();
	

	BLShader_Point.SetVector3f("viewPos", camera.Position);
	BLShader_Point.SetVector3f("LightPos", LightPos);
	BLShader_Point.SetVector3f("LightColor", LightColor);
	BLShader_Point.SetFloat("far_plane", far_plane);

	
	Texture2D gPosition=ResourceManager::GetTexture2D("gPosition");
	Texture2D gNormal =ResourceManager::GetTexture2D("gNormal");
	Texture2D gColorSpec =ResourceManager::GetTexture2D("gAlbedoSpec");
	Texture2D gTangentViewDir =ResourceManager::GetTexture2D("gTangentViewDir");
	Texture2D gTangentLightDir =ResourceManager::GetTexture2D("gTangentLightDir");

	glActiveTexture(GL_TEXTURE0);
	gPosition.Bind();

	glActiveTexture(GL_TEXTURE1);
	gNormal.Bind();

	glActiveTexture(GL_TEXTURE2);
	gColorSpec.Bind();

	glActiveTexture(GL_TEXTURE3);
	gTangentViewDir.Bind();

	glActiveTexture(GL_TEXTURE4);
	gTangentLightDir.Bind();

	glActiveTexture(GL_TEXTURE5);
	depthMap_Point.Bind();


	renderQuad();

	glBindFramebuffer(GL_READ_FRAMEBUFFER,gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
	glBlitFramebuffer(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,SCR_WIDTH,SCR_HEIGHT,GL_DEPTH_BUFFER_BIT,GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);


	Shader LightBoxShader = ResourceManager::GetShader("LightBoxShader");
	LightBoxShader.Use();
	LightBoxShader.SetMatrix4("projection", projection);
	LightBoxShader.SetMatrix4("view", view);

	LightBoxShader.SetVector3f("viewPos", camera.Position);
	LightBoxShader.SetVector3f("LightPos", LightPos);
	LightBoxShader.SetVector3f("LightColor", LightColor);
	//renderScene_LightSphere(LightBoxShader);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//------normal render end--------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	//-------Blur--------
	Shader HDR_Blur = ResourceManager::GetShader("HDR_Blur");
	Texture2D HDR_ColorBuffer1 = ResourceManager::GetTexture2D("HDR_ColorBuffer1");
	Texture2D pingpongColorBuffers[2] = {
	ResourceManager::GetTexture2D("pingpongColorBuffer0"),
	ResourceManager::GetTexture2D("pingpongColorBuffer1")
	};
	GLboolean horizontal = true, first_iteration = true;
	GLuint amount = 10;
	HDR_Blur.Use();
	glActiveTexture(GL_TEXTURE0);
	for (GLuint i = 0; i < amount; i++){
		glBindFramebuffer(GL_FRAMEBUFFER,pingpongFBO[horizontal]);
		HDR_Blur.SetInteger("horizontal", horizontal);
		glBindTexture(
			GL_TEXTURE_2D, first_iteration ? HDR_ColorBuffer1.ID : pingpongColorBuffers[!horizontal].ID
		);
		renderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//-------Blur end------





	//-------combind-------
	//render floating point color buffer to 2D quad and
	//tonemap HDR colors to default framebuffer's (clamped) color range
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	Texture2D HDR_ColorBuffer0 = ResourceManager::GetTexture2D("HDR_ColorBuffer0");

	Shader ShaderBloomFinal = ResourceManager::GetShader("ShaderBloomFinal");
	ShaderBloomFinal.Use();
	glActiveTexture(GL_TEXTURE0);
	HDR_ColorBuffer0.Bind();
	glActiveTexture(GL_TEXTURE1);
	pingpongColorBuffers[!horizontal].Bind();
	ShaderBloomFinal.SetInteger("bloom",false);
	ShaderBloomFinal.SetFloat("exposure",exposure);

	renderQuad();
	//Forward render

	glBindFramebuffer(GL_READ_FRAMEBUFFER,gBuffer);
	glBindFramebuffer(GL_DRAW_BUFFER,0);
	glBlitFramebuffer(0,0,SCR_WIDTH,SCR_HEIGHT,0,0,SCR_WIDTH,SCR_HEIGHT,GL_DEPTH_BUFFER_BIT,GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
	//

	TextureCube irradianceMap = ResourceManager::GetTextureCube("irradianceMap");
	TextureCube prefilterMap = ResourceManager::GetTextureCube("prefilterMap");
	Texture2D brdfLUTTexture = ResourceManager::GetTexture2D("brdfLUTTexture");

	Shader pbrShader = ResourceManager::GetShader("PbrShader");
	pbrShader.Use();
	pbrShader.SetMatrix4("view",view);
	pbrShader.SetMatrix4("projection", projection);
	pbrShader.SetVector3f("CamPos",camera.Position);

	glActiveTexture(GL_TEXTURE0);
	irradianceMap.Bind();
	glActiveTexture(GL_TEXTURE1);
	prefilterMap.Bind();
	glActiveTexture(GL_TEXTURE2);
	brdfLUTTexture.Bind();


	for (int row = 0; row < nrRows; ++row) {
		pbrShader.SetFloat("metallic",(float)row/(float)nrColumns);
		for (int col = 0; col < nrColumns; ++col)
		{
			pbrShader.SetFloat("roughness",glm::clamp((float)col/(float)nrColumns,0.05f,1.0f));

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(float)((col)-(nrColumns / 2))*spacing,
				(float)(row-(nrRows/2))*spacing,
				-2.0f
			));
			pbrShader.SetMatrix4("model",model);
			renderSphere();
		}
	}

	//Skybox render
	Shader SkyBoxShader = ResourceManager::GetShader("SkyBoxShader");

	renderScene_SkyBox(SkyBoxShader);

	


}

void Scene::renderScene_Point(Shader & shader)
{
	//demo shadow used
	shader.Use();
	shader.SetInteger("isModel", false);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.SetMatrix4("model", model);
	renderPlane1();


	//bottom
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	shader.SetMatrix4("model", model);
	renderPlane0();

	//right
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.5f, 1.0f));
	model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	shader.SetMatrix4("model", model);
	renderPlane0();

	//left
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.5f, -1.0f));
	model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
	shader.SetMatrix4("model",model);
	renderPlane0();

	//top
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shader.SetMatrix4("model", model);
	renderPlane0();

	//front
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(6.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMatrix4("model", model);
	renderPlane1();


	//back
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, -3.5f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	shader.SetMatrix4("model", model);
	renderPlane1();

	//NanoSuit
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -4.5f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model ,glm::vec3(0.1f));
	shader.SetMatrix4("model", model);
	shader.SetInteger("isModel",true);
	Model NaNoSuit=ResourceManager::GetModel("NaNoSuit");
	NaNoSuit.Draw(shader);
}

void Scene::renderScene_LightSphere(Shader & shader)
{
	shader.Use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, LightPos);
	model = glm::scale(model, glm::vec3(0.2f));
	shader.SetMatrix4("model", model);
	shader.SetVector3f("LightColor", LightColor);
	renderSphere();
}

void Scene::renderScene_SkyBox(Shader &shader) {
	glDepthFunc(GL_LEQUAL);

	shader.Use();
	shader.SetMatrix4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.SetMatrix4("projection", projection);
	TextureCube skyBox = ResourceManager::GetTextureCube("envCubeMap");
	glActiveTexture(GL_TEXTURE0);
	skyBox.Bind();
	glBindVertexArray(SkyBoxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDepthFunc(GL_LESS);
}



void Scene::Debug() {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	TextureCube irradianceMap = ResourceManager::GetTextureCube("irradianceMap");
	TextureCube prefilterMap = ResourceManager::GetTextureCube("prefilterMap");
	Texture2D brdfLUTTexture = ResourceManager::GetTexture2D("brdfLUTTexture");

	Shader pbrShader = ResourceManager::GetShader("PbrShader");
	pbrShader.Use();
	pbrShader.SetMatrix4("view", view);
	pbrShader.SetMatrix4("projection", projection);
	pbrShader.SetVector3f("CamPos", camera.Position);

	glActiveTexture(GL_TEXTURE0);
	irradianceMap.Bind();
	glActiveTexture(GL_TEXTURE1);
	prefilterMap.Bind();
	glActiveTexture(GL_TEXTURE2);
	brdfLUTTexture.Bind();


	for (int row = 0; row < nrRows; ++row) {
		pbrShader.SetFloat("metallic", (float)row / (float)nrColumns);
		for (int col = 0; col < nrColumns; ++col)
		{
			pbrShader.SetFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(float)(col)*spacing,
				(float)(row)*spacing,
				-2.0f
			));
			pbrShader.SetMatrix4("model", model);
			renderSphere();
		}
	}

	//Skybox render
	Shader SkyBoxShader = ResourceManager::GetShader("SkyBoxShader");

	renderScene_SkyBox(SkyBoxShader);


}