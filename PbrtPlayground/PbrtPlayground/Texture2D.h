#pragma once
#include<glad/glad.h>
#include"stb_image.h"
#include<string>
#include<iostream>
class Texture2D
{
public:

	GLuint ID;
	std::string type;
	std::string path;


	GLuint Width, Height;

	GLuint Internal_Format;
	GLenum Image_Format;
	GLenum DataType;
	// Texture configuration
	GLuint Wrap_S;
	GLuint Wrap_T;
	GLuint Filter_MIN; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_MAG; // Filtering mode if texture pixels > screen pixels

	Texture2D();
	Texture2D(GLuint width, GLuint height, GLuint InternalFormat = GL_RGB, GLenum ImageFormat = GL_RGB,
		GLenum DataType = GL_UNSIGNED_BYTE, GLuint Wrap = GL_CLAMP_TO_EDGE, GLuint Filter = GL_NEAREST);
	// Generates texture from image data
	void Generate(GLuint width, GLuint height, unsigned char* data, GLenum format);
	//@TODO:Generate void texture
	void Generate(GLuint width, GLuint height);
	void GenerateDepthMap(GLuint width, GLuint height);
	void GenerateHDRMap(GLuint width, GLuint height);
	void GenerateFromFile(std::string  path, bool gamma = false);
	void GenerateFromHDRFile(std::string path);
	// Binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
};

