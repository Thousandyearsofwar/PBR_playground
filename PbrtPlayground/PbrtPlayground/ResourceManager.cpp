#include "ResourceManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "stb_image.h"
// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures2D;
std::map<std::string, TextureCube>    ResourceManager::TexturesCube;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Model>       ResourceManager::Models;

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

Texture2D ResourceManager::LoadTexture2D(const GLchar *file, std::string name, GLboolean alpha,GLboolean hdrFormat)
{
	Texture2D texture;
	if(!hdrFormat)
		texture.GenerateFromFile(file, alpha);
	else
		texture.GenerateFromHDRFile(file);
	ResourceManager::Textures2D[name] = texture;
	return Textures2D[name];
}

Texture2D ResourceManager::LoadTexture2D(Texture2D texture, std::string name)
{
	ResourceManager::Textures2D[name] = texture;
	return texture;
}

TextureCube ResourceManager::LoadTextureCube(const vector<std::string>file, GLboolean alpha, std::string name)
{
	TextureCube texture;
	if (alpha){
		texture.Image_Format = GL_RGBA;
		texture.Internal_Format = GL_RGBA;
	}
	int nrComponents;
	std::vector<unsigned char*> images;
	int width, height;
	for (auto iter : file) {
		images.push_back(
			stbi_load(iter.c_str(), &width, &height, &nrComponents, 0)
		);
	}

	GLenum format;
	switch (nrComponents)
	{
	case 1:format = GL_RED; break;
	case 3:format = GL_RGB; break;
	case 4:format = GL_RGBA; break;
	}
	texture.Generate(width,height, images,format);
	for(auto iter:images)
		stbi_image_free(iter);
	ResourceManager::TexturesCube[name] = texture;
	return texture;
}

TextureCube ResourceManager::LoadTextureCube(TextureCube texture, std::string name)
{
	ResourceManager::TexturesCube[name] = texture;
	return texture;
}

Shader ResourceManager::LoadShader(const GLchar *vshaderFile, const GLchar* fshaderFile, const GLchar *gshderFile, std::string name) {
	ResourceManager::Shaders[name] = LoadShaderFromFile(vshaderFile,fshaderFile,gshderFile);
	return Shaders[name];
}

Model ResourceManager::LoadModel(const GLchar * file, GLboolean alpha, std::string name)
{
	ResourceManager::Models[name] = LoadModelFromFile(file, alpha);
	return Models[name];
}



Shader ResourceManager::LoadShaderFromFile(const GLchar *vShaderFile, const GLchar* fShaderFile, const GLchar *gShderFile)
{
	std::string vertexCode;
	std::string fragCode;
	std::string geomCode;

	try {
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragShaderFile(fShaderFile);
	
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragShaderFile.rdbuf();

		vertexShaderFile.close();
		fragShaderFile.close();

		vertexCode = vShaderStream.str();
		fragCode = fShaderStream.str();

		if (gShderFile != nullptr) {
			std::ifstream geomShaderFile(gShderFile);
			std::stringstream gShaderStream;
			gShaderStream << geomShaderFile.rdbuf();
			geomShaderFile.close();
			geomCode = gShaderStream.str();
		}
	}
	catch (std::exception e) {
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}

	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragCode.c_str();
	const char *gShaderCode = geomCode.c_str();
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShderFile != nullptr ? gShaderCode : nullptr);

	return shader;
}
Model ResourceManager::LoadModelFromFile(const GLchar * file, GLboolean alpha)
{
	return Model(file,alpha);
}



Texture2D ResourceManager::GetTexture2D(std::string name)
{
	return Textures2D[name];
}

TextureCube ResourceManager::GetTextureCube(std::string name)
{
	return TexturesCube[name];
}

Shader ResourceManager::GetShader(std::string name)
{
	return Shaders[name];
}

Model ResourceManager::GetModel(std::string name)
{
	return Models[name];
}


