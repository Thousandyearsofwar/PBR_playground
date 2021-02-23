#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H
#include<map>
#include<vector>
#include"Texture2D.h"
#include"TextureCube.h"
#include"Shader.h"
#include"Model.h"

class ResourceManager
{
private:
	static std::map<std::string, Model> Models;
	static std::map<std::string, Texture2D> Textures2D;
	static std::map<std::string, TextureCube> TexturesCube;
	static std::map<std::string, Shader> Shaders;

	static Shader LoadShaderFromFile(const GLchar *vshaderFile, const GLchar* fshaderFile, const GLchar *gshderFile = nullptr);
	static Model LoadModelFromFile(const GLchar *file, GLboolean alpha);


public:
	ResourceManager();
	~ResourceManager();
	static Texture2D LoadTexture2D(const GLchar *file, std::string name, GLboolean alpha = false,GLboolean hdrFormat =false);
	static Texture2D LoadTexture2D(Texture2D texture, std::string name);
	static TextureCube LoadTextureCube(const vector<std::string>file, GLboolean alpha, std::string name);
	static TextureCube LoadTextureCube(TextureCube texture, std::string name);

	static Shader LoadShader(const GLchar *vshaderFile, const GLchar* fshaderFile, const GLchar *gshderFile, std::string name);

	static Model LoadModel(const GLchar *file,GLboolean alpha,std::string name);

	static Texture2D GetTexture2D(std::string name);
	static TextureCube GetTextureCube(std::string name);
	static Shader GetShader(std::string name);
	static Model GetModel(std::string name);
};

#endif