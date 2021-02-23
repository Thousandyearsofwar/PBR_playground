#include "Texture2D.h"

Texture2D::Texture2D()
	: Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB),
	Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_MIN(GL_LINEAR), Filter_MAG(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

Texture2D::Texture2D(GLuint width , GLuint height, GLuint InternalFormat , GLenum ImageFormat ,
	GLenum DataType, GLuint Wrap, GLuint Filter ) :
	Width(width), Height(height), Internal_Format(InternalFormat), Image_Format(ImageFormat),
	Wrap_S(Wrap), Wrap_T(Wrap), Filter_MIN(Filter), Filter_MAG(Filter), DataType(DataType)
{
	glGenTextures(1, &this->ID);
}


void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data, GLenum format)
{
	this->Width = width;
	this->Height = height;
	this->Internal_Format = format;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Internal_Format, GL_UNSIGNED_BYTE, data);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Generate(GLuint width, GLuint height)
{
	this->Width = width;
	this->Height = height;
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Internal_Format, this->DataType, nullptr);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::GenerateDepthMap(GLuint width, GLuint height)
{
	glBindTexture(GL_TEXTURE_2D,this->ID);

	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT, width, height,0,GL_DEPTH_COMPONENT,GL_FLOAT,nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	GLfloat borderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::GenerateHDRMap(GLuint width, GLuint height)
{
	glBindTexture(GL_TEXTURE_2D, this->ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,this->Wrap_T);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::GenerateFromFile(std::string path, bool gamma)
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
	this->path = path;
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, this->ID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);


		this->Wrap_S= GL_REPEAT;
		this->Wrap_T= GL_REPEAT;
		this->Filter_MIN = GL_LINEAR_MIPMAP_LINEAR;
		this->Filter_MAG = GL_LINEAR;

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

}



void Texture2D::GenerateFromHDRFile(std::string path) {
	glBindTexture(GL_TEXTURE_2D,this->ID);
	this->path = path;
	this->Internal_Format = GL_RGB16F;
	this->Image_Format = GL_RGB;
	this->Wrap_S = GL_CLAMP_TO_EDGE;
	this->Wrap_T = GL_CLAMP_TO_EDGE;
	this->Filter_MIN = GL_LINEAR;
	this->Filter_MAG = GL_LINEAR;

	stbi_set_flip_vertically_on_load(true);

	int width=0, height=0, nrComponents;
	
	float  *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
	this->Width = width;
	this->Height = height;
	
	if (data) {
		
		glTexImage2D(GL_TEXTURE_2D,0, this->Internal_Format,width,height,0, this->Image_Format,GL_FLOAT,data);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);
	
		stbi_image_free(data);
	}
	else {
		std::cout << "Failed to load HDR image." << std::endl;
	}
	stbi_set_flip_vertically_on_load(false);
}

void Texture2D::Bind() const {
	glBindTexture(GL_TEXTURE_2D, this->ID);
}