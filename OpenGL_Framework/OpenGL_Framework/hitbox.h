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

	void update(int t);
	vec3 getPosition();
	vec3 getVelocity();
	float getSize();
	void setPosition(vec3 pos);
	bool isDone();
	float getAngle();
	float getKnockback();
	void setDone();
	mat4 getTransform();

private:
	//physics
	vec3 acceleration;
	vec3 position;
	vec3 velocity;

	//Hitbox Tings
	float knockback;
	float angleDeg;
	float size;
	unsigned int owner;

	//actions
	unsigned int activeFrames;
	unsigned int currentFrame;

};