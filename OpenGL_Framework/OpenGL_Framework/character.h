
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
#include "MiniMath/Core.h"
//#include "camera.h"
#include "hitbox.h"
#include <iostream>
#include "Transform.h"


#define ACTION_IDLE				0
#define ACTION_WALK				1
#define ACTION_RUN				2
#define ACTION_INTIAL_DASH		3
#define ACTION_PREJUMP			4
#define ACTION_JUMP				5
#define ACTION_JUMP2			6
#define ACTION_FALL				7
#define ACTION_HIT				8
#define ACTION_PLACEHOLDER		9

#define ACTION_JAB				10
#define ACTION_SIDE_ATTACK		11
#define ACTION_DOWN_ATTACK		12
#define ACTION_UP_ATTACK		13
#define ACTION_NEUTRAL_SPECIAL	14
#define ACTION_SIDE_SPECIAL		15
#define ACTION_DOWN_SPECIAL		16
#define ACTION_UP_SPECIAL		17

#define ACTION_NEUTRAL_AERIAL	20
#define ACTION_SIDE_AERIAL		21
#define ACTION_DOWN_AERIAL		22
#define ACTION_UP_AERIAL		23

#define ACTION_BLOCK			30
#define ACTION_DASH				31


//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Character {

public:
	Character() {}
	Character(const std::string& body, const std::string& texture);

	void update(int t, std::vector<bool> inputs);
	void draw(ShaderProgram GBufferPass, float dt);
	void drawBoxes(ShaderProgram GBufferPass);
	void drawShadow(ShaderProgram GBufferPass, float dt);
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
	Transform initialDash(bool left, bool right);
	Transform prejump();
	Transform jump();
	Transform jump2();
	Transform fall();
	void hit(Hitbox* hitBy);
	Transform jab();
	Transform sAttack();
	Transform dAttack();
	Transform uAttack();
	Transform nSpecial(bool charging);
	Transform sSpecial();
	Transform dSpecial();
	Transform uSpecial();
	Transform nAir();
	Transform sAir();
	Transform dAir();
	Transform uAir();
	Transform block(bool held);

	//----------------------------------------------------------
	void comboAdd() {
		if (comboTimer < comboMaxTime) {
			comboCount++;
			comboMeter += (2 + (int)(comboCount));//amount added to combo
			resetTimer();
		}
		else {
			comboClear();
			resetTimer();
		}
		std::cout << "Count: " << comboCount << " Meter: " << comboMeter << std::endl;
	}
	void comboClear() { comboCount = 0; std::cout << comboCount << std::endl; }
	void resetMeter() { comboMeter = 0; comboClear(); }
	void comboTick() {
		if (comboMeter < 0) {
			comboMeter = 0;
		}
		comboTimer++;
	}
	void resetTimer() { comboTimer = 0; }

	void respawn() {
		comboTimer = 0;
		comboMeter = 0;
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
	std::vector<Mesh*> idleFrames;
	std::vector<Mesh*> walkFrames;
	std::vector<Mesh*> jabFrames;
	std::vector<Mesh*> sAtkFrames;
	std::vector<Mesh*> uAtkFrames;
	std::vector<Mesh*> dAtkFrames;
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

private:

};