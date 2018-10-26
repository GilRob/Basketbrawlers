#include "Texture.h"
#include "SOIL\SOIL.h"

#include <iostream>

Texture::~Texture()
{
	Unload();
}

bool Texture::Load(const std::string &file)
{
	TextObj = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (TextObj == 0)
	{
		std::cout << "Texture failed to load.\n" << SOIL_last_result() << "\n";
		return false;
	}

	//Send the data to OpenGL
	glBindTexture(GL_TEXTURE_2D, TextObj); //BInd the new texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//S and T are some sort of axes?
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;
}

void Texture::Unload()
{
	if (TextObj != 0)
	{
		//Remove date from GPU
		glDeleteTextures(1, &TextObj);
		TextObj = 0;
	}
}

void Texture::SetNearestFilter()
{
	glBindTexture(GL_TEXTURE_2D, TextObj);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, TextObj);
}

void Texture::UnBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}