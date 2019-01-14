#pragma once
//#include <SDL/SDL.h>
//#include <glm/glm.hpp>
//#include <GL/freeglut.h>
//#include <GL/glut.h>
//#include <GL/glew.h>
//#include <GL/gl.h>
#include <glm/glm.hpp>
#include <GLM\gtx\transform.hpp>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
//#include "camera.h"
#include "hitbox.h"
#include <iostream>
#include "Transform.h"


//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Character {

public:
	Character() {}
	Character(const std::string& body, const std::string& texture);

	void update(int t, std::vector<bool> inputs);
	void draw(ShaderProgram GBufferPass, float dt);
	void drawBoxes(ShaderProgram GBufferPass);
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	std::vector<Hitbox*> getHitboxes();
	std::vector<Hitbox*> getHurtboxes();
	Transform atkInputHandler(std::vector<bool> inputs);

	bool facingRight;
	bool blocking;
	bool blockSuccessful;

	//actions
	unsigned int action;
	unsigned int activeFrames;
	unsigned int currentFrame;
	bool interuptable;

	//Actions
	Transform idle();
	Transform walk(bool held);
	Transform run(bool held);
	Transform dash(bool left, bool right);
	Transform initialDash(bool left, bool right);
	Transform prejump();
	Transform jump();
	Transform jump2();
	Transform fall();
	void hit(Hitbox* hitBy);
	virtual Transform jab();
	virtual Transform sAttack();
	virtual Transform dAttack();
	virtual Transform uAttack();
	virtual Transform nSpecial(bool charging);
	virtual Transform sSpecial();
	virtual Transform dSpecial();
	virtual Transform uSpecial();
	virtual Transform nAir();
	virtual Transform sAir();
	virtual Transform dAir();
	virtual Transform uAir();
	Transform block(bool held);

	//----------------------------------------------------------
	void comboAdd() {
		if (comboTimer < comboMaxTime) {
			comboCount++;
			//comboMeter += (2 + (int)(comboCount));//amount added to combo
			resetTimer();
		}
		else {
			comboClear();
			resetTimer();
		}
		comboMeter += (2 + (int)(comboCount));//amount added to combo
		std::cout << "Count: " << comboCount << " Meter: " << comboMeter << std::endl;
	}
	void comboClear() { comboCount = 0; }
	void resetMeter() { comboMeter = 0; comboClear(); }
	void comboTick() {
		if (comboMeter < 0) {
			comboMeter = 0;
		}
		comboTimer++;
	}
	void resetTimer() { comboTimer = 0; }

	void respawn() {
		comboTimer = 50;
		comboMeter = 50;
		position = glm::vec3(0, 15, 0);
		velocity = glm::vec3(0, 0, 0);
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		fall();
	}

	int getMeter() {
		return comboMeter;
	}

	Transform transform;
	Mesh body;
	std::vector<Mesh*> aniFrames[23];
	Texture texture;
	float aniTimer;
	unsigned int index;
protected:
	//model

	//physics
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
	glm::vec3 hitForce;

	//Attributes
	float mass;
	float gravity;
	float runSpeed;//max
	float runAccel;
	float airAccel;
	float jumpForce;
	float diMultiplier;
	float dashMultiplier;
	unsigned int jumpFrames;
	unsigned int dashLength;
	unsigned int prejumpLength;
	unsigned int airJumps;
	int jumpsLeft;
	int hitstun;
	int hitframes;

	//combo stuff
	unsigned int comboCount;//counts hits in a row, resets after x time
	int comboMeter;//total value of combo meter from 0-100
	unsigned int comboTimer;//counts time since last hit in combo
	unsigned int comboMaxTime;//2 seconds times 60fps
	float comboHitMult;//multipy combo meter value by this to get knockback

	unsigned int dashTimer;//counts time since last hit in combo

	//scaling
	float scaleX;
	float scaleY;
	float scaleZ;

	//attacks
	std::vector<Hitbox*> activeHitboxes;
	std::vector<Hitbox*> hurtbox;//2


	//debug hitbox
	Mesh boxMesh;
	Texture boxTexture;
	Texture shieldTexture;

//===================================================//
	//ACTION ID's
//===================================================//
unsigned int ACTION_IDLE			=0;
unsigned int ACTION_WALK			=1;
unsigned int ACTION_RUN				=2;
unsigned int ACTION_INTIAL_DASH		=3;
unsigned int ACTION_PREJUMP			=4;
unsigned int ACTION_JUMP			=5;
unsigned int ACTION_JUMP2			=6;
unsigned int ACTION_FALL			=7;
unsigned int ACTION_HIT				=8;
unsigned int ACTION_PLACEHOLDER		=23;
unsigned int ACTION_JAB				=10;
unsigned int ACTION_SIDE_ATTACK		=11;
unsigned int ACTION_DOWN_ATTACK		=12;
unsigned int ACTION_UP_ATTACK		=13;
unsigned int ACTION_NEUTRAL_SPECIAL	=14;
unsigned int ACTION_SIDE_SPECIAL	=15;
unsigned int ACTION_DOWN_SPECIAL	=16;
unsigned int ACTION_UP_SPECIAL		=17;
unsigned int ACTION_NEUTRAL_AERIAL	=18;
unsigned int ACTION_SIDE_AERIAL		=19;
unsigned int ACTION_DOWN_AERIAL		=20;
unsigned int ACTION_UP_AERIAL		=21;
unsigned int ACTION_BLOCK			=9;
unsigned int ACTION_DASH			=22;
//===================================================//

private:
};