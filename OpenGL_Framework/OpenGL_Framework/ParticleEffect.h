#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Transform.h"

struct ParticleList
{
	glm::vec3 *Positions = nullptr;
	glm::vec3 *Velocities = nullptr;
	float *Size = nullptr;
	float *Alpha = nullptr;
	float *Ages = nullptr;
	float *Lifetimes = nullptr;
};

class ParticleEffect
{
public:
	ParticleEffect();
	~ParticleEffect();

	bool Init(const std::string &textureFile, unsigned int maxParticles, unsigned int rate);

	void Update(float elapsed);
	void Render();
	void Reset();

	Transform transform;

	glm::vec2 RangeX = glm::vec2(-10.0f, 10.0f);
	glm::vec2 RangeY = glm::vec2(0.0f, 10.0f);
	glm::vec2 RangeZ = glm::vec2(-10.0f, 10.0f);
	glm::vec2 RangeVelocity = glm::vec2(0.1f, 1.0f);
	glm::vec2 RangeLifetime = glm::vec2(7.0f, 9.5f);
	//Lerp variables are dynamic based on lifetime of particle
	glm::vec2 LerpAlpha = glm::vec2(0.5f, 0.0f);
	glm::vec2 LerpSize = glm::vec2(0.0f, 1.0f);

	bool HaveGravity = false;
	bool Playing = false;
	float Mass;
	float Gravity;
	glm::vec3 force;
	glm::vec3 acceleration;

private:
	ParticleList _Particles;
	Texture _Texture;

	float _Rate = 0.0f;
	unsigned int _MaxParticles = 0;
	unsigned int _NumCurrentParticles = 0;

	GLuint _VAO = 0;
	GLuint _VBO_Position = 0;
	GLuint _VBO_Size = 0;
	GLuint _VBO_Alpha = 0;
};
