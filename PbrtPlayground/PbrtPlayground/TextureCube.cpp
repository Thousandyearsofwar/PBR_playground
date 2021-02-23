#include "TextureCube.h"



TextureCube::TextureCube():
	Width(0),Height(0),Internal_Format(GL_RGB),Image_Format(GL_RGB),
	Wrap_S(GL_CLAMP_TO_EDGE),Wrap_T(GL_CLAMP_TO_EDGE),Wrap_R(GL_CLAMP_TO_EDGE),
	Filter_MIN(GL_LINEAR), Filter_MAG(GL_LINEAR), DataType(GL_UNSIGNED_BYTE)
{
	glGenTextures(1, &this->ID);
}

TextureCube::TextureCube(GLuint width, GLuint height, GLuint InternalFormat, GLenum ImageFormat,
	GLenum DataType, GLuint Wrap, GLuint Filter):
	Width(width), Height(height), Internal_Format(InternalFormat), Image_Format(ImageFormat),
	Wrap_S(Wrap), Wrap_T(Wrap), Wrap_R(Wrap),
	Filter_MIN(Filter), Filter_MAG(Filter), DataType(DataType)
{
	glGenTextures(1, &this->ID);
}


TextureCube::~TextureCube()
{
}

void TextureCube::Generate(GLuint width, GLuint height, std::vector<unsigned char*>data, GLenum format)
{
	this->Width = width;
	this->Height = height;
	this->Internal_Format = format;

	glBindTexture(GL_TEXTURE_CUBE_MAP,this->ID);
	GLuint size = data.size();
	for(GLuint i=0;i<size;i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, this->Internal_Format, width, height, 0, this->Image_Format, this->DataType, data[i]);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->Wrap_R);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);

	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}

void TextureCube::Generate(GLuint width, GLuint height, GLboolean mipMap )
{
	this->Width = width;
	this->Height = height;
	

	glBindTexture(GL_TEXTURE_CUBE_MAP, this->ID);
	
	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, this->Internal_Format, width, height, 0, this->Image_Format, this->DataType, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->Wrap_R);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);

	if (mipMap)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	
}

void TextureCube::GenerateDepthCubeMap(GLuint width, GLuint height)
{
	this->Width = width;
	this->Height = height;
	
	glBindTexture(GL_TEXTURE_CUBE_MAP,this->ID);

	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,this->Internal_Format,width,height,0,this->Image_Format, this->DataType,NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, this->Wrap_R);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, this->Filter_MIN);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, this->Filter_MAG);

}

void TextureCube::Bind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP,this->ID);
}
