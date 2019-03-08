#pragma once
#include <vector>
#include <GL/glew.h>
#include "VertexBufferObject.h"
#include "UniformBuffer.h"
#include "Texture.h"

/*
  /////////////////
 // Framebuffer //
/////////////////

FrameBuffers are a collection of buffers (usually textures) used as a method of
storing the results of rendering. OpenGL has 2 different kinds of framebuffers,
one that we've been using this whole time! The Default Framebuffer, used to
represent a window or display device, and user-created Framebuffer Objects.
"FBOs" reference images from either Textures or Renderbuffers, and are not
directly visible. In order to view them, we must draw/blit the framebuffer
to the Default Framebuffer. In order to draw it, we create a quad that covers
the entire screen, and sample from the FBO like a texture using a simple
passthrough shader. A Framebuffer can hold any number of color attachments
and an optional depth attachment. No color and only depth is also allowed.
They can also contain a stencil buffer, or even a merged depth/stencil.

More info about Framebuffers can be read here
https://www.khronos.org/opengl/wiki/Framebuffer

Limitations of Framebuffers can be found using glGetIntegerv(GL_MACRO, &integer)
	-Framebuffers have a maximum number of draw buffers
		GL_MAX_DRAW_BUFFERS
*/

struct DepthTarget
{
	~DepthTarget();
	void unload();
	Texture _Tex;
};

struct ColorTarget
{
	~ColorTarget();
	void unload();
	std::vector<Texture> _Tex;
	std::vector<GLenum> _Format;
	std::vector<GLenum> _Buffer;
	unsigned int _NumAttachments = 0;
};

class Framebuffer
{
public:
	~Framebuffer();
	void unload();
	void init(unsigned width, unsigned height);
	void init();
	void setSize(unsigned width, unsigned height);
	void addDepthTarget();
	void addColorTarget(GLenum format);
	void reshape(unsigned width, unsigned height);

	void setViewport() const;
	void Bind() const;
	void UnBind() const;
	void renderToFSQ() const;
	void bindResolution();

	void bindColorAsTexture(unsigned colorBuffer, int textureSlot) const;
	void bindDepthAsTexture(int textureSlot) const;
	void unbindTexture(int textureSlot) const;

	void backbuffer();
	bool checkFBO();

	void Clear();

	GLuint _FBO; // OpenGL Handle
	DepthTarget _Depth; // Depth Attachment (one or none)
	ColorTarget _Color; // Color Attachments (can be more than 1)
	bool _IsInit = false;
	bool _DepthActive = false;

	GLenum _Filter = GL_NEAREST;
	GLenum _Wrap = GL_CLAMP_TO_EDGE;

	unsigned _Width = 0;
	unsigned _Height = 0;

	GLbitfield clearFlag = 0;

	static void initFrameBuffers();
	static void drawFSQ();
protected:
	UniformBuffer _ResolutionUBO;
	static int _MaxColorAttachments;
	static glm::ivec3 _MaxFramebufferSize; // Width, Height, and Layers (Depth) texture dimensions
	static bool _IsInitFrameBuffers;
	static VertexArrayObject FullScreenQuad;
};

class Gbuffer : public Framebuffer
{
public:
	Gbuffer();
	void setDebugNames();

	std::vector<Texture*> textures;
};



//Frame bufferes can only have one depth texture but can have multiple colour textures
/*
#pragma once
#include <GL\glew.h>
#include "IO.h"
#include "Texture.h"
#include <vector>

class FrameBuffer
{
public:
	//This give a compilation error if you use the default constructor
	FrameBuffer() = delete;
	//The int represents the number of colour attachments the class should expect to have, to allocate the right amount of space
	FrameBuffer(unsigned numColorAttachments);
	~FrameBuffer();

	void InitDepthTexture(unsigned width, unsigned height);
	void InitColorTexture(unsigned index, unsigned width, unsigned height, GLint internalFormat, GLint filter, GLint wrap);
	bool CheckFBO();

	//- Clears all OpenGL memory
	void Unload();

	//- Clears all attached textures
	void Clear();

	void Bind();
	void UnBind();

	void MoveToBackBuffer(int windowWidth, int windowHeight);

	//Very important
	GLuint GetDepthHandle() const;
	GLuint GetColorHandle(unsigned index) const;

private:
	GLuint _FBO = GL_NONE;
	GLuint _DepthAttachment = GL_NONE;
	GLuint *_ColorAttachments = nullptr;
	//A variabe to hold the parameters for afunction later on...?
	GLenum *_Bufs = nullptr;

	unsigned int _NumColorAttachments = 0;

};*/