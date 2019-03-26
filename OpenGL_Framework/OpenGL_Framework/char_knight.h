#pragma once
#include "character.h"

//Will be the parent class for all other charcaters
///Has all basic functions/data that charcaters need but lacks the unique passives and thas virtual functions for each attack type
class Knight : public Character {

public:
	Knight() {}
	Knight(const std::string& body, const std::string& texture);

	Knight(const Knight* copy) {
		type = 1;

		this->aniTimer = 0.0f;
		this->index = 0;

		this->body = copy->body;
		this->bodyTexture = copy->bodyTexture;
		this->hurtTexture = copy->hurtTexture;
		activeTexture = &(this->bodyTexture);
		this->boxMesh = copy->boxMesh;
		this->boxTexture = copy->boxTexture;
		this->shieldTexture = copy->shieldTexture;


		//duplicate animations
		for (int i = 0; i < 23; i++) {//for each action
			for (int j = 0; j < (int)(copy->aniFrames[i].size()); j++) {//for each pose
				this->aniFrames[i].push_back(new Mesh(*copy->aniFrames[i][j]));//add pose to this character list
			}
		}
		for (int i = 0; i < (int)(aniFrames->size()); i++) {//for each action
			for (int j = 0; j < (int)(aniFrames[i].size()); j++) {//for each pose
				this->aniFrames[i][j]->VAO = copy->aniFrames[i][j]->VAO;
				this->aniFrames[i][j]->VBO_Normals = copy->aniFrames[i][j]->VBO_Normals;
				this->aniFrames[i][j]->VBO_UVs = copy->aniFrames[i][j]->VBO_UVs;
				this->aniFrames[i][j]->VBO_Vertices = copy->aniFrames[i][j]->VBO_Vertices;
			}
		}


		//Set Physics
		position = glm::vec3(0, 0, 0);
		velocity = glm::vec3(0, 0, 0);
		lastPos = glm::vec3(0, 0, 0);
		acceleration = glm::vec3(0, 0, 0);
		force = glm::vec3(0, 0, 0);
		facingRight = true;
		ultMode = false;
		//scaling
		scaleX = copy->scaleX;
		scaleY = copy->scaleY;
		scaleZ = copy->scaleZ;

		transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
		//glm::rotate(transform, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.RotateY(90);

		mass = copy->mass;
		gravity = copy->gravity;
		diMultiplier = copy->diMultiplier;
		dashMultiplier = copy->dashMultiplier;
		runSpeed = copy->runSpeed;
		runAccel = copy->runAccel;
		airAccel = copy->airAccel;
		jumpForce = copy->jumpForce;
		jumpFrames = copy->jumpFrames;
		dashLength = copy->dashLength;
		prejumpLength = copy->prejumpLength;
		airJumps = jumpsLeft = 1;
		hitstun = copy->hitstun;
		hitframes = copy->hitframes;

		//set combo stuff
		comboCount = 0;
		comboMeter = 0;
		comboTimer = 0;
		comboMaxTime = 120;//1 seconds times 60fps

		//Set Starting Action
		action = ACTION_FALL;//0 idle, 1 jumping
		idle();


		comboMeter = copy->comboMeter;

		Hitbox *hurt1 = new Hitbox(glm::vec3(0.0f, 3.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt1);
		Hitbox *hurt2 = new Hitbox(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
		hurtbox.push_back(hurt2);
	}



	int storedCharge;

	//Attributes
	float ultMass = 10;
	float ultGravity = 0.7f;
	float ultRunSpeed = 1.0f;
	float ultRunAccel = 0.5f;
	float ultAirAccel = 0.1f;
	float ultJumpForce = 0.8f;
	float ultDashMultiplier = 1.2f;
	float ultScaleX = 1.5f;
	float ultScaleY = 1.5f;
	float ultScaleZ = 1.5f;
	float normalScaleX;
	float normalScaleY;
	float normalScaleZ;
	float normMass;
	float normGravity;
	float normRunSpeed;
	float normRunAccel;
	float normAirAccel;
	float normJumpForce;
	float normDashMultiplier;


	void update(int t, std::vector<bool> inputs) {
		//check for ult
		if (inputs[9] && comboMeter > 0 && !ultMode) {
			ultMode = true;
			partiQueue.push(ULTFX);						//$$$
			ultFrame1 = true;
		}

		if (ultMode) {
			//drain and stop mode
				comboMeter -= 0.5f;
			if (comboMeter <= 0)
				ultMode = false;

			//change change values
			if (ultMass != mass) {
				normMass = mass;
				mass = ultMass;
			}
			if (ultGravity != gravity) {
				normGravity = gravity;
				gravity = ultGravity;
			}
			if (ultRunSpeed != runSpeed) {
				normRunSpeed = runSpeed;
				runSpeed = ultRunSpeed;
			}
			if (ultRunAccel != runAccel) {
				normRunAccel = runAccel;
				runAccel = ultRunAccel;
			}
			if (ultAirAccel != airAccel) {
				normAirAccel = airAccel;
				airAccel = ultAirAccel;
			}
			if (ultJumpForce != jumpForce) {
				normJumpForce = jumpForce;
				jumpForce = ultJumpForce;
			}
			if (ultDashMultiplier != dashMultiplier) {
				normDashMultiplier = dashMultiplier;
				dashMultiplier = ultDashMultiplier;
			}
			if (ultScaleX != scaleX) {
				normalScaleX = scaleX;
				scaleX = ultScaleX;
			}
			if (ultScaleY != scaleY) {
				normalScaleY = scaleY;
				scaleY = ultScaleY;
			}
			if (ultScaleZ != scaleZ) {
				normalScaleZ = scaleZ;
				scaleZ = ultScaleZ;
			}
		}
		else {
			if (ultMass == mass) {
				mass = normMass;
			}
			if (ultGravity == gravity) {
				gravity = normGravity;
			}
			if (ultRunSpeed == runSpeed) {
				runSpeed = normRunSpeed;
			}
			if (ultRunAccel == runAccel) {
				runAccel = normRunAccel;
			}
			if (ultAirAccel == airAccel) {
				airAccel = normAirAccel;
			}
			if (ultJumpForce == jumpForce) {
				jumpForce = normJumpForce;
			}
			if (ultDashMultiplier == dashMultiplier) {
				dashMultiplier = normDashMultiplier;
			}
			if (ultScaleX == scaleX)
			{
				scaleX = normalScaleX;
			}
			if (ultScaleY == scaleY)
			{
				scaleY = normalScaleY;
			}
			if (ultScaleZ == scaleZ)
			{
				scaleZ = normalScaleZ;
			}
		}
		Character::update(t, inputs);
	}

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

protected:

private:
};