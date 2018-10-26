#pragma once
#include <GL\glew.h>
#include <string>

class Texture
{
public:
	Texture() = default;
	~Texture();

	bool Load(const std::string &file);
	void Unload();

	void SetNearestFilter();

	void Bind();
	void UnBind();

	//- THe handle to the texture object
	GLuint TextObj = 0;
};