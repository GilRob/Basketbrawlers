#include "hitbox.h"

Hitbox::Hitbox(vec3 _position, float _size, float _knockback, float _angleDeg, unsigned int _activeFrames, unsigned int _owner, vec3 _velocity)
{
	//set all values
	position = _position;
	size = _size;
	knockback = _knockback;
	angleDeg = _angleDeg;
	velocity = _velocity;
	activeFrames = _activeFrames;
	currentFrame = 1;
	owner = _owner;
	acceleration = vec3(0, 0, 0);
	projectile = false;
	spline = false;

}

Hitbox::~Hitbox() {

}
/*
//updates the position
void Hitbox::update(int t)
{
	if (currentFrame <= activeFrames) {

		//update position
		velocity = velocity + (acceleration);
		position = position + (velocity);
		currentFrame++;
	}
}*/

//updates the position
void Hitbox::update(int t, vec3 parent = { 0,0,0 })
{
	if (currentFrame <= activeFrames) {
		//update position
		if (spline) {
			if (curve.size() <= 4)
				position = catmull(curve[0], curve[1], curve[2], curve[3], (float)currentFrame / activeFrames);
			else if(curve.size() == 5){
				float percent = (float)currentFrame / activeFrames;
				if (percent < 0.5f)
					position = catmull(curve[0], curve[1], curve[2], curve[3], percent - 0.25);
				else
					position = catmull(curve[1], curve[2], curve[3], curve[4], percent - 0.75f);


			}

			if (projectile) {
				globalPosition = position;
			}
			else {
				globalPosition = position + parent;
			}
		}
		else {
			velocity = velocity + (acceleration);
			position = position + (velocity);
			if (projectile) {
				globalPosition = position;
			}
			else {
				globalPosition = position + parent;
			}
		}
		currentFrame++;
	}
}

//Returns the position of the hitbox
vec3 Hitbox::getPosition()
{
	return globalPosition;
}

//Returns the position of the hitbox
mat4 Hitbox::getTransform()
{
	mat4 trans;
	trans.SetTranslation(globalPosition);
	trans.Scale(vec3(size,size,size));
	return trans;
}


//Returns the position of the hitbox
vec3 Hitbox::getVelocity()
{
	return velocity;
}

//returns the size of the hitbox
float Hitbox::getSize()
{
	return size;
}

//sets position
void Hitbox::setPosition(vec3 pos)
{
	globalPosition = pos;
}

//returns true if active frames are up
bool Hitbox::isDone()
{
	if (currentFrame <= activeFrames)
		return false;
	else
		return true;
}

void Hitbox::setDone()
{
	currentFrame = activeFrames + 1;
}

float Hitbox::getAngle()
{
	return angleDeg;
}

float Hitbox::getKnockback()
{
	return knockback;
}
