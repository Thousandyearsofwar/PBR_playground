#pragma once
#include<glad/glad.h>
#include<vector>
class TextureCube
{
public:
	
	GLuint ID;

	GLuint Width, Height;

	GLuint Internal_Format;
	GLenum Image_Format;
	GLenum  DataType;
	//Texture Configuration
	GLuint Wrap_S;
	GLuint Wrap_T;
	GLuint Wrap_R;

	GLuint Filter_MIN;// Filtering mode if texture pixels < screen pixels
	GLuint Filter_MAG;// Filtering mode if texture pixels > screen pixels

	TextureCube();
	TextureCube(GLuint width, GLuint height, GLuint InternalFormat = GL_RGB, GLenum ImageFormat = GL_RGB,
		GLenum DataType = GL_UNSIGNED_BYTE, GLuint Wrap = GL_CLAMP_TO_EDGE, GLuint Filter = GL_NEAREST);
	~TextureCube();

	void Generate(GLuint width,GLuint height,std::vector<unsigned char*>data,GLenum format);
	void Generate(GLuint width, GLuint height,GLboolean mipMap = false);
	void GenerateDepthCubeMap(GLuint width, GLuint height);
	void Bind() const;
};

