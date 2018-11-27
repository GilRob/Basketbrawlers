#pragma once

#include <vector>
#include "MiniMath\Core.h"
#include "Texture.h"

struct ParticleList
{
	vec3 *Positions = nullptr;
	vec3 *Velocities = nullptr;
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

	mat4 Transform;

	vec2 RangeX = vec2(-10.0f, 10.0f);
	vec2 RangeY = vec2(0.0f, 10.0f);
	vec2 RangeZ = vec2(-10.0f, 10.0f);
	vec2 RangeVelocity = vec2(0.1f, 1.0f);
	vec2 RangeLifetime = vec2(7.0f, 9.5f);
	//Lerp variables are dynamic based on lifetime of particle
	vec2 LerpAlpha = vec2(0.5f, 0.0f);
	vec2 LerpSize = vec2(0.0f, 1.0f);

	bool HaveGravity = false;
	bool Playing = false;
	float Mass;
	float Gravity;
	vec3 force;
	vec3 acceleration;

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
