#pragma once
#include "GL/glew.h"
#include <string>
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>

/*
  //////////////
 // Textures //
//////////////

Textures are used to hold one or more images of the same format,
they can either be used for reading from inside a Shader, of used
as a render target for drawing (Done with the Framebuffer class)

Common types of textures are
GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, and GL_TEXTURE_CUBE_MAP

Mipmaps are pre-shrunk versions of a full-sized image, and help prevent aliasing
with far away/zoomed out textures. OpenGL will automatically select which mipmap
layer to use, and prevent the need to take multiple texture samples per fragment.

You can read more about Textures here
https://www.khronos.org/opengl/wiki/Texture
There are many parameters for Texture sampling, such as Filtering and Edge Wrapping
These parameters can even be shared across multiple textures using Sampler Objects
https://www.khronos.org/opengl/wiki/Sampler_Object

Limitations of Textures can be found using glGetIntegerv(GL_MACRO, &integer)
	-Textures have a limit on their resolution for any dimension
		GL_MAX_TEXTURE_SIZE
		GL_MAX_3D_TEXTURE_SIZE
		GL_MAX_ARRAY_TEXTURE_LAYERS

	-Texture sizes are advised to stick to powers of 2
		16, 32, 64, 128, 256, 512, 1024, etc.

*/

struct TextureFilter
{
	GLenum mag = GL_LINEAR;
	GLenum min = GL_LINEAR_MIPMAP_LINEAR;
};

GLenum filterModes[];

#pragma warning(push)
#pragma warning(disable : 4201) // Nameless struct warning
struct TextureWrap
{
	TextureWrap();

	union
	{
		struct
		{
			GLenum x, y, z;
		};
		struct
		{
			GLenum s, t, r;
		};
	};
};
#pragma warning(pop)

class TextureHandle
{
protected:
	GLuint _Handle = 0;

public:

	TextureHandle() = default;
};

class Texture : public TextureHandle
{
public:
	friend class DepthTarget;
	friend class ColorTarget;
	friend class Framebuffer;
	friend class GBuffer;

	Texture() = default;
	Texture(const std::string &file, bool mipmap = true, bool sRGB = false);
	~Texture();

	// Loads the texture
	bool Load(const std::string &file, bool mipmap = true, bool sRGB = false);
	// Unloads the texture
	bool Unload();

	// Description:
	// Creates the texture, allocates memory and uploads data to GPU
	// If you do not want to upload data to the GPU pass in a nullptr for the dataPtr.
	// For a description on filtering and edgeBehaviour see https://www.khronos.org/opengles/sdk/docs/man/xhtml/glTexParameter.xml
	// For a description on internalFormat, textureFormat and dataType see https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
	void createTexture(int w, int h, GLenum target, GLenum filtering, GLenum edgeBehaviour, GLenum internalFormat, GLenum textureFormat, GLenum dataType, void* newDataPtr);

	// Binds the texture to the last active slot
	void Bind() const;
	// Binds the texture to a specific slot
	void Bind(int textureSlot) const;
	// Unbinds the last active texture slot
	void UnBind() const;
	// Unbinds a specific texture slot
	void UnBind(int textureSlot) const;
	GLuint getID();

	void sendTexParameters();
	void setFilterParameters(GLenum mag, GLenum min);
	void setWrapParameters(GLenum wrap);

	static float anisotropyAmount;
protected:
	int countMipMapLevels(bool mipmap);
	void generateMipMaps();

	std::string filename;
	std::string filetype;

	GLenum _InternalFormat;
	GLenum _Format;
	GLenum _Target;
	TextureFilter _Filter;
	TextureWrap _Wrap;

	int sizeX;
	int sizeY;
	int channels;

	static std::string _TextureDirectory;
	static std::string _TextureCubeDirectory;
};

/*#pragma once
#include <GL\glew.h>
#include <string>

class Texture
{
public:
	Texture() = default;
	~Texture();

	bool Load(const std::string &file);
	bool Unload();

	void SetNearestFilter();

	void Bind() const;
	void Bind(int texSlot) const;
	void UnBind() const;
	void UnBind(int texSlot) const;

	//- The handle to the texture object
	GLuint TextObj = 0;

private:
	GLuint _TexHandle;

	std::string filename;

	GLenum _InternalFormat;
	GLenum _Target;
	GLenum _FilterMag = GL_LINEAR; // Filter parameter
	GLenum _FilterMin = GL_LINEAR_MIPMAP_LINEAR;// GL_LINEAR;
	GLenum _WrapU = GL_REPEAT; // Per axis Wrap parameter
	GLenum _WrapV = GL_REPEAT;

	int sizeX;
	int sizeY;
	int channels;
};*/