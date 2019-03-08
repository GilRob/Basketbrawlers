#include "Texture.h"
//#include <gmtk/gmtk.h>
#include "SOIL/SOIL.h"
#include "IO.h"

std::string Texture::_TextureDirectory = "../assets/textures/";
std::string Texture::_TextureCubeDirectory = _TextureDirectory + "cubemap/";

float Texture::anisotropyAmount = 16.0f;

GLenum filterModes[] =
{
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

Texture::Texture(const std::string & file, bool mipmap, bool sRGB)
{
	this->Load(file, mipmap);
}

Texture::~Texture()
{
	this->Unload();
}

bool Texture::Load(const std::string & file, bool mipmap, bool sRGB)
{
	this->filename = file;

	unsigned char* textureData = SOIL_load_image((this->filename).c_str(),
		&this->sizeX, &this->sizeY, &this->channels, SOIL_LOAD_RGBA);

	if (this->sizeX <= 0 || this->sizeY <= 0 || this->channels <= 0)
	{
		SAT_DEBUG_LOG_ERROR("TEXTURE BROKE: %s", this->filename.c_str());
		return false;
	}

	// If the texture is 2D, set it to be a 2D texture
	_Target = GL_TEXTURE_2D;
	// Force the texture to be RGBA
	if (sRGB)
	{
		_Format = GL_RGBA;
		_InternalFormat = GL_SRGB8_ALPHA8;
	}
	else
	{
		_Format = GL_RGBA;
		_InternalFormat = GL_RGBA8;
	}

	// Calculate number of mipmaps, so the memory can be allocated before generating them
	int levels = countMipMapLevels(mipmap);

	glGenTextures(1, &this->_Handle);
	this->Bind();
	// Allocate the memory of the texture, using it's format and dimensions
	glTextureStorage2D(this->_Handle, levels, this->_InternalFormat, this->sizeX, this->sizeY);
	// Send the data of the texture
	glTextureSubImage2D(this->_Handle, 0, // We are editing the first layer in memory (Regardless of mipmaps)
		0, 0, // No offset
		this->sizeX, this->sizeY, // the dimensions of our image loaded
		_Format, GL_UNSIGNED_BYTE, // Data format and type
		textureData); // Pointer to the texture data

	if (mipmap)
	{
		generateMipMaps();
	}

	glTextureParameteri(this->_Handle, GL_TEXTURE_MIN_FILTER, this->_Filter.min);
	glTextureParameteri(this->_Handle, GL_TEXTURE_MAG_FILTER, this->_Filter.mag);
	glTextureParameteri(this->_Handle, GL_TEXTURE_WRAP_S, this->_Wrap.x);
	glTextureParameteri(this->_Handle, GL_TEXTURE_WRAP_T, this->_Wrap.y);

	this->UnBind();
	SOIL_free_image_data(textureData);
	setOpenGLName(GL_TEXTURE, _Handle, file);
	return true;
}

bool Texture::Unload()
{
	// Delete the texture handle if it exists
	if (this->_Handle)
	{
		glDeleteTextures(1, &this->_Handle);
		return true;
	}
	return false;
}

int Texture::countMipMapLevels(bool mipmap)
{
	int levels = 1;

	if (mipmap)
	{
		float largest = static_cast<float>(glm::max(this->sizeX, this->sizeY));
		levels += static_cast<int>(std::floor(std::log2(largest)));
	}
	return levels;
}

void Texture::generateMipMaps()
{
	glGenerateTextureMipmap(this->_Handle);
	glTextureParameterf(this->_Handle, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropyAmount);
}

void Texture::createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void * newDataPtr)
{
	sizeX = w;
	sizeY = h;
	_Filter.mag = filtering;
	_Wrap.x = edgeBehaviour;
	_Wrap.y = edgeBehaviour;
	_InternalFormat = internalFormat;
	_Target = target;

	GLenum error = 0;

	// Not necessary to enable GL_TEXTURE_* in modern context.
	//	glEnable(m_pTarget);
	//	error = glGetError();

	Unload();

	glGenTextures(1, &_Handle);
	glBindTexture(target, _Handle);
	error = glGetError();

	glTexParameteri(_Target, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(_Target, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(_Target, GL_TEXTURE_WRAP_S, edgeBehaviour);
	glTexParameteri(_Target, GL_TEXTURE_WRAP_T, edgeBehaviour);
	error = glGetError();

	glTexImage2D(_Target, 0, internalFormat, w, h, 0, textureFormat, dataType, newDataPtr);
	error = glGetError();

	if (error != 0)
	{
		SAT_DEBUG_LOG_ERROR("[Texture.cpp : createTexture] Error when creating texture. ");
	}

	glBindTexture(_Target, 0);
}

void Texture::Bind() const
{
	glBindTexture(this->_Target, this->_Handle);
}

void Texture::Bind(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	this->Bind();
}

void Texture::UnBind() const
{
	glBindTexture(this->_Target, GL_NONE);
}

void Texture::UnBind(int textureSlot) const
{
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	this->UnBind();
}

GLuint Texture::getID()
{
	return _Handle;
}

void Texture::sendTexParameters()
{
	glTextureParameteri(this->_Handle, GL_TEXTURE_MIN_FILTER, this->_Filter.min);
	glTextureParameteri(this->_Handle, GL_TEXTURE_MAG_FILTER, this->_Filter.mag);
	glTextureParameteri(this->_Handle, GL_TEXTURE_WRAP_S, this->_Wrap.x);
	glTextureParameteri(this->_Handle, GL_TEXTURE_WRAP_T, this->_Wrap.y);
}

void Texture::setFilterParameters(GLenum mag, GLenum min)
{
	_Filter.mag = mag;
	_Filter.min = min;
}

void Texture::setWrapParameters(GLenum wrap)
{
	_Wrap.x = wrap;
	_Wrap.y = wrap;
	_Wrap.z = wrap;
}

TextureWrap::TextureWrap()
{
	x = GL_REPEAT;
	y = GL_REPEAT;
	z = GL_REPEAT;
}


/*#include "Texture.h"
#include "SOIL\SOIL.h"

#include <iostream>

Texture::~Texture()
{
	Unload();
}

bool Texture::Load(const std::string &file)
{
	/*TextObj = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

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

	return true;*/
	
	//this->filename = "./Assets/Textures/" + file; //We should use this later
	
	//Uncomment from here******
	/*this->filename = file;

	unsigned char* textureData;
	textureData = SOIL_load_image((this->filename).c_str(),
		&(this->sizeX), &(this->sizeY), &(this->channels), SOIL_LOAD_RGBA);

	if (this->sizeX == 0 || this->sizeY == 0 || this->channels == 0)
	{
		//SAT_DEBUG_LOG_ERROR("TEXTURE BROKE: %s", this->filename.c_str());
		return false;
	}

	// If the texture is 2D, set it to be a 2D texture;
	_Target = GL_TEXTURE_2D;
	_InternalFormat = GL_RGBA8;

	glGenTextures(1, &this->_TexHandle);
	this->Bind();
	glTextureStorage2D(this->_TexHandle, 1, this->_InternalFormat, this->sizeX, this->sizeY);

	glTextureSubImage2D(this->_TexHandle, 0, // We are editing the first and only layer in memory
		0, 0, // No offset
		this->sizeX, this->sizeY, // the dimensions of our image loaded
		GL_RGBA, GL_UNSIGNED_BYTE, // Data format and type
		textureData); // Pointer to the texture data

	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MAG_FILTER, this->_FilterMag);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_MIN_FILTER, this->_FilterMin);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_S, this->_WrapU);
	glTextureParameteri(this->_TexHandle, GL_TEXTURE_WRAP_T, this->_WrapV);

	this->UnBind();
	SOIL_free_image_data(textureData);
	return true;
}

bool Texture::Unload()
{
	if (this->_TexHandle != 0)
	{
		//Remove date from GPU
		glDeleteTextures(1, &this->_TexHandle);
		return true;
	}
	return false;
}

void Texture::SetNearestFilter()
{
	glBindTexture(GL_TEXTURE_2D, this->_TexHandle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Texture::Bind() const
{
	glBindTexture(this->_Target, this->_TexHandle);
}

void Texture::Bind(int texSlot) const
{
	glActiveTexture(GL_TEXTURE0 + texSlot);
	this->Bind();
}

void Texture::UnBind() const
{
	glBindTexture(this->_Target, GL_NONE);
}

void Texture::UnBind(int texSlot) const
{
	glActiveTexture(GL_TEXTURE0 + texSlot);
	this->UnBind();
}*/
