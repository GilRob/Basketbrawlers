
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


//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Character{

public:
	Character() {}
	Character(const std::string& body, const std::string& texture);

	void update(int t, std::vector<bool> inputs);
	void draw(ShaderProgram GBufferPass);
	void drawShadow(ShaderProgram GBufferPass);
	vec3 getPosition();
	void setPosition(vec3 pos);
	std::vector<Hitbox*> getHitboxes();
	void hit(Hitbox* hitBy);
	mat4 atkInputHandler(std::vector<bool> inputs);

	//AttackFunctions
	mat4 jab();
	mat4 fTilt();
	mat4 dTilt();
	//virtual void uTilt();
	mat4 nAir();
	mat4 fAir();
	//Transform bAir();
	mat4 uAir();
	mat4 dAir();
	mat4 nSpecial(bool charging);
	mat4 sSpecial();
	mat4 dSpecial();
	mat4 uSpecial();

	/*
	virtual void sideB();
	virtual void upB();
	virtual void downB();
	virtual void neutBAir();
	virtual void sideBAir();
	virtual void upBAir();
	virtual void downBAir();*/
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
	void comboClear() { comboCount = 0; std::cout << comboCount << std::endl;}
	void resetMeter() { comboMeter = 0; comboClear();}
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