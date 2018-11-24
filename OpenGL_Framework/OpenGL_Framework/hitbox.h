//#include <SDL/SDL.h>
//#include <glm/glm.hpp>
#include "mesh.h"
//#include "shader.h"
#include "texture.h"
//#include "transform.h"
#include <vector>
//#include "camera.h"
#include "MiniMath/Core.h"


class Hitbox {

public:
	Hitbox() {}
	Hitbox(vec3 _position, float _size, float _knockback, float _angleDeg, unsigned int _activeFrames, unsigned int owner, vec3 _velocity);
	~Hitbox();

	void update(int t, vec3 parent);
	vec3 getPosition();
	vec3 getVelocity();
	float getSize();
	void setPosition(vec3 pos);
	bool isDone();
	float getAngle();
	float getKnockback();
	void setDone();
	mat4 getTransform();
	bool spline;
	bool facingRight;
	bool projectile;
	vec3 acceleration;
	std::vector<vec3> curve;


	vec3 catmull(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float t)
	{
		return 0.5f * (
			t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
			t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
			t * (-p0 + p2) +
			(2.0f * p1));
	}

	vec4 catmull(vec4 p0, vec4 p1, vec4 p2, vec4 p3, float t)
	{
		return 0.5f * (
			t * t * t * (-p0 + 3.0f * p1 - 3.0f * p2 + p3) +
			t * t * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) +
			t * (-p0 + p2) +
			(2.0f * p1));
	}



private:
	//physics
	vec3 position;
	vec3 velocity;
	vec3 globalPosition;

	//Hitbox Tings
	float knockback;
	float angleDeg;
	float size;
	unsigned int owner;

	//actions
	unsigned int activeFrames;
	unsigned int currentFrame;

};