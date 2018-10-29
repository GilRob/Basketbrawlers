
//#include <SDL/SDL.h>
//#include <glm/glm.hpp>
//#include <GL/freeglut.h>
//#include <GL/glut.h>
//#include <GL/glew.h>
//#include <GL/gl.h>
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include <vector>
#include "MiniMath/Core.h"
//#include "camera.h"
#include "hitbox.h"
#include <iostream>


#define ACTION_IDLE				0
#define ACTION_WALK				1
#define ACTION_RUN				2
#define ACTION_DASH				3
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


//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Character {

public:
	Character() {}
	Character(const std::string& body, const std::string& texture);

	void update(int t, std::vector<bool> inputs);
	void draw(ShaderProgram GBufferPass);
	void drawShadow(ShaderProgram GBufferPass);
	vec3 getPosition();
	void setPosition(vec3 pos);
	std::vector<Hitbox*> getHitboxes();
	mat4 atkInputHandler(std::vector<bool> inputs);

	mat4 idle();
	mat4 walk();
	mat4 run();
	mat4 dash();
	mat4 prejump();
	mat4 jump();
	mat4 jump2();
	mat4 fall();
	void hit(Hitbox* hitBy);
	mat4 jab();
	mat4 sAttack();
	mat4 dAttack();
	mat4 uAttack();
	mat4 nSpecial(bool charging);
	mat4 sSpecial();
	mat4 dSpecial();
	mat4 uSpecial();
	mat4 nAir();
	mat4 sAir();
	mat4 dAir();
	mat4 uAir();

	void comboAdd() {
		if (comboTimer < comboMaxTime) {
			comboCount++;
			comboMeter += (int)(comboCount);
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
	void comboTick() { comboTimer++; }
	void resetTimer() { comboTimer = 0; }

	void respawn() {
		comboTimer = 0;
		comboMeter = 0;
		position = vec3(0, 15, 0);
		velocity = vec3(0, 0, 0);
		action = 1;
	}

	mat4 transform;
protected:
	//model
	Mesh body;
	//Shader* shader;
	Texture texture;

	//physics
	vec3 position;
	vec3 velocity;
	vec3 acceleration;
	vec3 force;
	float mass;
	float gravity;
	int upwardsForce;
	float runspeed;//max speed
	float runaccel;//accel multiplyer
	float jumpforce;//velocity going up
	float diMultiplier;//multiplier for DI when in hitstun
	unsigned int jumpframes;//length of jump

	vec3 hitforce;//velocity going up

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

	//actions
	unsigned int action;//0 idle, 1 jumping, 2-jab, 3-fTilt, 4-dTilt
	//0-idle, 1-jumping
	//2-jab, 3-fTilt, 4-dTilt, 5-uTilt
	//6-nAir, 7-fAir, 8-dAir, 9-uAir
	//10-hit
	//11-nSpecial, 12-sSpecial, 13-dSpecial, 14-uSpecial
	unsigned int activeFrames;
	unsigned int currentFrame;
	bool interuptable;

	//looking
	bool facingRight;

	//debug hitbox
	Mesh boxMesh;
	//Shader* boxShader;
	Texture boxTexture;

private:

};