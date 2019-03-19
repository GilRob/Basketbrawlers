#include "character.h"

/*
Keys:
$$$ - Particle Signal

*/

//new push
#define BASE_ANI_TOGGLE		false	//non-offensive animations
#define G_ATK_ANI_TOGGLE	false	//ground attacks
#define A_ATK_ANI_TOGGLE	false	//aerials
#define S_ATK_ANI_TOGGLE	false	//specials
#define HITBOX_TOGGLE		false	//visual hitboxes
#define HURTBOX_TOGGLE		false	//visual hurtboxes

Character::Character(const std::string& bodyName, const std::string& textureName){

	//Nothing Here.  DO NOT MAKE CHARACTER

}

void Character::update(int t, std::vector<bool> inputs) {

	//max meter
	if (comboMeter > 200)
		comboMeter = 200;

	//Change Player Facing only during these actinos.
	if (action <= 7 && action != ACTION_INTIAL_DASH) {
		///check the way it should face
		if ((int)inputs[1] - (int)inputs[3] > 0) {
			facingRight = true;
		}
		else if ((int)inputs[1] - (int)inputs[3] < 0) {
			facingRight = false;
		}
	}

	//COMBO METER
	comboTick();

	//meter flame
	if ((comboMeter > 150 && action != ACTION_HIT && action != ACTION_IN_NET) || action == ACTION_RESPAWN || ultMode) {
		partiQueue.push(METERFLAME);						//$$$
	}

	//actual update
	force.x = 0;
	transform = atkInputHandler(inputs);
	if (action != ACTION_DASH && action != ACTION_IN_NET && action != ACTION_RESPAWN) {

		//physics update
		force = glm::vec3(force.x, 0 - gravity, 0);
		acceleration = force / mass;
		velocity = velocity + (acceleration);

		//max speed
		if (velocity.x > runSpeed)
			velocity.x = runSpeed;
		if (velocity.x < (0 - runSpeed))
			velocity.x = (0 - runSpeed);

		//friction
		if (position.y <= 0.0f && (((int)inputs[1] - (int)inputs[3]) == 0 || (action != ACTION_WALK && action != ACTION_RUN && action != ACTION_INTIAL_DASH && action != ACTION_PREJUMP && action != ACTION_JUMP))) {
			if (action != ACTION_SIDE_ATTACK)
				velocity.x = velocity.x * 0.7f;
			else
				velocity.x = velocity.x * 0.95f;

		}
		if (position.y > 0.0f && !inputs[1] && !inputs[3]) {
			velocity.x = velocity.x * 0.95f;
		}
		
		if (dashTimer < 100)
			dashTimer++;
	}

	//Update Position
	position = position + (velocity);

	//update movement dir
	pos2d = glm::vec2(position.x, position.y);
	movementDir = (pos2d - lastPos);
	lastPos = pos2d;

	///Rotate the player to the correct way they should look
	if (facingRight == true)
		//glm::rotate(transform, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.RotateY(0);
	else
		//glm::rotate(transform, 180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		transform.RotateY(180);

	//Fake Wall
	///Will be changed later
	if (abs(position.x) > 25.1f) {
		//called on bounce
		position.x = (position.x / abs(position.x)) * 25.1f;
		if (action == ACTION_HIT) {
			velocity.y *= 0.75f;
			velocity.x *= -0.75f;
			hitForce.x *= -0.9f;
		}
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL)) {
			//interuptable = true;
			//action = ACTION_PLACEHOLDER;
			//idle();
			velocity.x *= 0.1f;
		}
	}

	//Fake Roof Code
	///Will be changed later
	if (position.y > 19.0f && action != ACTION_RESPAWN) {
		//called on landing
		position.y = 19.0f;
		if (action == ACTION_HIT) {
			velocity.y *= -0.75f;
			velocity.x *= 0.9f;
			hitForce.y *= -0.9f;
		}
		else {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
			velocity.y *= -0.5f;
		}
	}

	//Fake Floor Code
	///Will be changed later
	if (position.y < 0.0f) {
		//called on landing
		position.y = 0.0f;
		jumpsLeft = airJumps;
		if (action == ACTION_HIT && currentFrame < activeFrames) {
			velocity.y *= -0.75f;
			velocity.x *= 0.5f;
			hitForce.y *= -0.9f;
		}
		else if (action == ACTION_HIT)
		{
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
		}
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL || action == ACTION_SIDE_AERIAL || action == ACTION_UP_AERIAL || action == ACTION_DOWN_AERIAL || action == ACTION_NEUTRAL_AERIAL)) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			idle();
			//velocity.x *= 0.7f;
			partiQueue.push(LANDDUST);						//$$$
		}
		else if (currentFrame < activeFrames - 5 && (action == ACTION_SIDE_AERIAL || action == ACTION_UP_AERIAL || action == ACTION_DOWN_AERIAL || action == ACTION_NEUTRAL_AERIAL)) {//landing lag
			currentFrame = activeFrames - 3;
			index = aniFrames[action].size() -1;
			//velocity.x *= 0.7f;
		}
	}
	else {
		//called after hitstun is up in the air
		if (currentFrame >= activeFrames && action == ACTION_HIT) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
		}
	}
	
	//Testing Code for Spawning Hitboxes
	//Check Hitboxes
	for (unsigned int i = 0; i < activeHitboxes.size(); i++) {
		activeHitboxes[i]->update(t, position);

		if (activeHitboxes[i]->isDone())
			activeHitboxes.erase(activeHitboxes.begin() + i);
	}

	//Check Hurtboxes
	for (unsigned int i = 0; i < hurtbox.size(); i++) {
		hurtbox[i]->update(t, position);
	}

	transform.SetTranslation(glm::vec3(position.x, position.y, position.z));
	//glm::scale(transform, glm::vec3(scaleX, scaleY, scaleZ));
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));

}

//Returns the Players Position
glm::vec3 Character::getPosition()
{
	return position;
}

void Character::draw(ShaderProgram shader, float dt) {
	if (action <= ACTION_DASH) {
		if (action == ACTION_IDLE || action == ACTION_SIDE_SPECIAL || action == ACTION_UP_SPECIAL)
			aniTimer += dt / 5.0f;
		else
			aniTimer += dt / 3.1f;
		while (aniTimer > 1.0f)
		{
			aniTimer -= 1.0f;
			index = (index + 1) % (aniFrames[action].size());//9 total frames
		}
		if (index >= aniFrames[action].size())
			index = 0;
		// Ask for the handles identfying the uniform variables in our shader.
		shader.SendUniformMat4("uModel", transform.data, true);
		shader.SendUniform("interp", aniTimer);

		int modelLoc = glGetUniformLocation(shader.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, transform.data);

		activeTexture->Bind();
		glBindVertexArray(aniFrames[action][index]->VAO);

		// Adjust model matrix for next object's location
		glDrawArrays(GL_TRIANGLES, 0, aniFrames[action][index]->GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	}
	else {
		index = 0;
		shader.SendUniformMat4("uModel", transform.data, true);
		shader.SendUniform("interp", 0);

		int modelLoc = glGetUniformLocation(shader.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, transform.data);

		activeTexture->Bind();
		glBindVertexArray(aniFrames[ACTION_IDLE][index]->VAO);

		glDrawArrays(GL_TRIANGLES, 0, aniFrames[ACTION_IDLE][index]->GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	}
}


void Character::drawBoxes(ShaderProgram GBufferPass) {
	//hurtboxes
	if (HURTBOX_TOGGLE) {
		for (unsigned int i = 0; i < hurtbox.size(); i++) {
			int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
			glUniformMatrix4fv(modelLoc, 1, false, hurtbox[i]->getTransform().data);

			shieldTexture.Bind();
			glBindVertexArray(boxMesh.VAO);

			// Adjust model matrix for next object's location
			glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
			glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
		}
		shieldTexture.UnBind();
	}
	//hitboxes
	if (HITBOX_TOGGLE) {
		for (unsigned int i = 0; i < activeHitboxes.size(); i++) {
			int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
			glUniformMatrix4fv(modelLoc, 1, false, activeHitboxes[i]->getTransform().data);

			boxTexture.Bind();
			glBindVertexArray(boxMesh.VAO);

			// Adjust model matrix for next object's location
			glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
			glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
		}
		boxTexture.UnBind();
	}

}

//Sets player position
void Character::setPosition(glm::vec3 pos){
	position = pos;
}

//Returns a list of all active hitboxes from this player
std::vector<Hitbox*> Character::getHitboxes()
{
	return activeHitboxes;
}

std::vector<Hitbox*> Character::getHurtboxes()
{
	return hurtbox;
}

//called on hit
void Character::hit(Hitbox* hitBy) {
	float result;
	if (!hitBy->facingRight) 
		result = 0.06f;
	else 
		result = -0.06f;

	facingRight = !(hitBy->facingRight);
	float xComp = hitBy->getKnockback() * cos((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);
	float yComp = hitBy->getKnockback() * sin((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);

	glm::vec3 add(result * xComp, 0.06f * yComp, 0);
	hitForce = add;
	action = ACTION_HIT;
	interuptable = false;
	activeFrames = unsigned int(hitframes + hitstun + hitBy->getKnockback());//
	currentFrame = 1;
}

///0-up, 1-left, 2-down, 3-right, 4-A, 5-B, 6-jump
///7-hardLeft, 8-hardRight
///9-shield
Transform Character::atkInputHandler(std::vector<bool> inputs)
{
	Transform result;
	///die
	if (action == ACTION_RESPAWN) {
		if (currentFrame == 1) {//on first frame pause
			comboTimer = 50;
			comboMeter = 50;
			activeTexture = &(bodyTexture);
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			//position.x = 0;
			position.y = 20;
			currentFrame = 1;
		}
		//If in Hitstun reduce directional influence
		if (currentFrame != 1 && currentFrame < 50) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			//position.x = 0;
			position.y -= 0.2f;
			if((int)(currentFrame * 0.2f) % 2)
				activeTexture = &(bodyTexture);
			else
				activeTexture = &(hurtTexture);
		}
		else if (currentFrame >= 50) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			activeTexture = &(bodyTexture);
			//respawn();
			comboTimer = 50;
			comboMeter = 50;
			//position = glm::vec3(0, 10, 0);
			velocity = glm::vec3(0, 0, 0);
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			fall();
		}

		if (facingRight)
			result.RotateY(-45.0f + currentFrame * 6.0f);
		else
			result.RotateY(-45.0f - currentFrame * 6.0f);

		currentFrame++;
	}
	else if (action == ACTION_IN_NET) {
		if (activeTexture == &bodyTexture) {//on first frame pause
			activeTexture = &(hurtTexture);
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			position.x = (position.x / abs(position.x)) * 25;
			position.y = 13 - (2.0f * (cosf((float)currentFrame * 0.157f) ));
			position.x += 1.2f * sinf((float)currentFrame * 0.157f);
			result.RotateZ((float)(currentFrame) * 9);
			currentFrame = 1;
		}
		//If in Hitstun reduce directional influence
		if (currentFrame != 1 && currentFrame < 80) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			position.x = (position.x / abs(position.x)) * 25;
			position.y = 13 - (2.0f * (cosf((float)currentFrame * 0.157f)));
			position.x += 1.2f * sinf((float)currentFrame * 0.157f);
			result.RotateZ((float)(currentFrame) * 9);
		}
		else if (currentFrame >= 80) {
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			activeTexture = &(bodyTexture);
			currentFrame = 1;
			activeFrames = 50;
			position.x = 0;
			respawn();
		}

		if (facingRight)
			result.RotateY(-45.0f);
		else
			result.RotateY(45.0f);

		currentFrame++;
		if (action == ACTION_RESPAWN)
			currentFrame = 1;
	}
	///hit
	else if (action == ACTION_HIT) {
		//If in Hitstun reduce directional influence
		if (currentFrame != 1) {
			force.x = (inputs[1] - inputs[3]) *  diMultiplier;
			if (currentFrame < (unsigned int)hitframes) {//only launched for hitframes, character will just be stunned for the remaining frames (hitstun + moves kb)
				velocity = hitForce = hitForce * 0.99f;			
				partiQueue.push(LAUNCHDUST);				//$$$
			}
			else
				result.RotateZ((float)(currentFrame - hitframes)*(-0.5f + (int)facingRight));
		}
		else {//on first frame pause
			force = glm::vec3(0, 0, 0);
			acceleration = glm::vec3(0, 0, 0);
			velocity = glm::vec3(0, 0, 0);
			if (facingRight) 
				partiQueue.push(HITSPARKL);				//$$$
			else
				partiQueue.push(HITSPARKR);				//$$$
		}
		if (facingRight)
			result.RotateY(-45.0f);
		else
			result.RotateY(45.0f);

		currentFrame++;
	}
	///dash
	else if ((inputs[10] && (action == ACTION_IDLE || action == ACTION_WALK || action == ACTION_RUN || action == ACTION_INTIAL_DASH || action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_DASH) {

		result = dash(inputs[1], inputs[3]);
	}
	//AIR
	else if ((inputs[2] && inputs[4] && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_DOWN_AERIAL) {//down & A and in air = Dair
		result = dAir();
	}
	else if ((inputs[0] && inputs[4] && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_UP_AERIAL) {//up & A and in air = Uair
		result = uAir();
	}
	else if ((((inputs[1] || inputs[3]) && inputs[4]) && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_SIDE_AERIAL) {//left or right & A in air = fair
		result = sAir();
	}
	else if ((inputs[4] && (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)) || action == ACTION_NEUTRAL_AERIAL) {//just A in air = nair
		result = nAir();
	}
	///jump
	else if (action == ACTION_JUMP) {
		result = jump();
		force.x = (inputs[1] - inputs[3])* 0.5f *  airAccel;
		if (inputs[7] || inputs[8])
			force.x *= 2.0f;
	}
	///jump2
	else if ((inputs[6] && action == ACTION_FALL) || action == ACTION_JUMP2) {
		result = jump2();
		force.x = (inputs[1] - inputs[3])* 0.5f *  airAccel;
		if (inputs[7] || inputs[8])
			force.x *= 2.0f;
	}
	///fall
	else if (action == ACTION_FALL) {
		result = fall();
		force.x = (inputs[1] - inputs[3])* 0.5f *  airAccel;
		if (inputs[7] || inputs[8])
			force.x *= 2.0f;
	}
	///prejump
	else if ((inputs[6] && (action == ACTION_IDLE || action == ACTION_RUN || action == ACTION_WALK || action == ACTION_INTIAL_DASH)) || action == ACTION_PREJUMP) {
		result = prejump();
	}
	//GROUNDED
	else if ((inputs[2] && inputs[4]) || action == ACTION_DOWN_ATTACK) {//down & A = Dtilt
		result = dAttack();
	}
	else if ((inputs[0] && inputs[4]) || action == ACTION_UP_ATTACK) {//down & A = Dtilt
		result = uAttack();
	}
	else if (((inputs[1] || inputs[3]) && inputs[4]) || action == ACTION_SIDE_ATTACK) {//left or right & A = Ftilt
		result = sAttack();
	}
	else if ((inputs[4]) || action == ACTION_JAB) {//just A = jab
		result = jab();
	}
	//SPECIALS
	//else if ((inputs[2] && inputs[5]) || action == ACTION_DOWN_SPECIAL) {//down + B = down Special
	//	result = dSpecial();
	//}
	//else if ((inputs[0] && inputs[5]) || action == ACTION_UP_SPECIAL) {//up + B = up Special
	//	result = uSpecial();
	//}
	//else if (((inputs[1] || inputs[3]) && inputs[5]) || action == ACTION_SIDE_SPECIAL) {//left or right + B = side Special
	//	result = sSpecial();
	//}
	//else if ((inputs[5]) || action == ACTION_NEUTRAL_SPECIAL) {//just B = Neutral Special
	//	result = nSpecial(inputs[5]);
	//}
	//NON-OFFENSIVE
	///intial dash
	else if (((((inputs[7] || inputs[8]) && (action == ACTION_IDLE || (action == ACTION_WALK && currentFrame < activeFrames*0.5f))) || action == ACTION_INTIAL_DASH) && action != ACTION_RUN)) {
		
		result = initialDash(inputs[7], inputs[8]);
	}
	///run
	else if ((inputs[7] || inputs[8]) && (action == ACTION_WALK || action == ACTION_RUN)) {

		//dashdancing
		if (facingRight && inputs[8]) {
			velocity.x *= -0.75f;
			facingRight = false;
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			result = initialDash(inputs[7], inputs[8]);
		}
		if (!facingRight && inputs[7]) {
			velocity.x *= -0.75f;
			facingRight = true;
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			result = initialDash(inputs[7], inputs[8]);
		}
		else
			result = run(inputs[7] || inputs[8]);
	}
	///walk
	else if ((inputs[1] || inputs[3]) && (action == ACTION_WALK || action == ACTION_IDLE)) {
		result = walk(inputs[1] || inputs[3]);
	}
	///idle
	else if (!(inputs[1] || inputs[3]) || action == ACTION_IDLE) {
		result = idle();
	}

	return result;

}

Transform Character::idle()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_IDLE) {
		action = ACTION_IDLE;
		activeFrames = 27;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_IDLE && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}

		//stuff goes here

		//std::cout << "idle" << std::endl;
		
		currentFrame++;
	}
	return result;
}

Transform Character::walk(bool held)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_WALK) {
		action = ACTION_WALK;
		activeFrames = 42;
		currentFrame = 1;
		interuptable = true;
	}
	else if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return idle();
	}
	else if (action == ACTION_WALK && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto walk
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return walk(held);
		}

		//max speed
		float mag = velocity.x;
		if (velocity.x > runSpeed*0.3f)
			velocity.x = runSpeed * 0.3f;
		if (velocity.x < (0 - (runSpeed * 0.3f)))
			velocity.x = (0 - (runSpeed * 0.3f));

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;
		force.x = direction * runAccel * 0.3f;
		//std::cout << "walk" << std::endl;

		currentFrame++;
	}
	return result;
}

Transform Character::run(bool held)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_RUN) {
		action = ACTION_RUN;
		activeFrames = 30;
		currentFrame = 1;
		interuptable = true;
	}
	else if (!held) {
		interuptable = true;
		action = ACTION_PLACEHOLDER;
		return walk(true);
	}
	else if (action == ACTION_RUN && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto walk
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return run(held);
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;

		force.x = direction * runAccel;
		//std::cout << "run" << std::endl;

		currentFrame++;
	}
	return result;
}

Transform Character::initialDash(bool right, bool left)
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_INTIAL_DASH) {
		action = ACTION_INTIAL_DASH;
		activeFrames = dashLength;
		currentFrame = 1;
		interuptable = true;

		int direction = (int)facingRight;
		if (facingRight == 0) {
			direction = -1;
		}



	}
	else if (action == ACTION_INTIAL_DASH && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto run
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return run(true);
		}
		

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0) {
			direction = -1;
			partiQueue.push(RDASHDUST);						//$$$
		}
		else {
			partiQueue.push(LDASHDUST);						//$$$
		}
		//dashdancing
		if (direction == 1 && left == true) { 
			velocity.x *= -0.1f; 
			direction *= -1;
			facingRight = false;
			currentFrame = 1;
		}
		if (direction == -1 && right == true) {
			velocity.x *= -0.1f;
			direction *= -1;
			facingRight = true;
			currentFrame = 1;
		}

		force.x = direction * runAccel;

		currentFrame++;
		//std::cout << " dash" << std::endl;
	}
	return result;
}


Transform Character::dash(bool right, bool left)
{
	Transform result = Transform();
	//if (interuptable == true && action != ACTION_DASH) {
	if (action != ACTION_DASH && dashTimer > 30) {
		action = ACTION_DASH;
		activeFrames = 10;
		currentFrame = 1;

		dashTimer = 0;
		
		interuptable = true;

		//decide which way youll dash
		if (left) {
			velocity.y = 0.0f;
			velocity.x = runSpeed * -2.0f;
			facingRight = false;
		}
		else if (right) {
			velocity.y = 0.0f;
			velocity.x = runSpeed * 2.0f;
			facingRight = true;
		}
	}
	else if (action == ACTION_DASH && currentFrame <= activeFrames) {

		if (currentFrame >= activeFrames) {
			//if action over, goto run
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			if (position.y > 0.0f)
				return fall();
			else
				return idle();
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;

		velocity.y = 0.0f;
		velocity.x = direction * runSpeed * dashMultiplier;


		//force.x = direction * runAccel;

		currentFrame++;
		//std::cout << " dash" << std::endl;
	}
	return result;
}

Transform Character::prejump()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_PREJUMP) {
		action = ACTION_PREJUMP;
		activeFrames = prejumpLength;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_PREJUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto jump
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return jump();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}

Transform Character::jump()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMP) {
		action = ACTION_JUMP;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		velocity.x *= 0.9f;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpForce;

		currentFrame++;
	}
	return result;
}

Transform Character::jump2()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JUMP2 && jumpsLeft > 0) {
		action = ACTION_JUMP2;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		jumpsLeft--;
		velocity.x *= 0.5f;
		partiQueue.push(LANDDUST);						//$$$
	}
	else if (action == ACTION_JUMP2 && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto fall
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpForce;

		currentFrame++;
	}
	return result;
}

Transform Character::fall()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_FALL) {
		action = ACTION_FALL;
		activeFrames = 9;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_FALL && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			action = ACTION_PLACEHOLDER;
			interuptable = true;
			return fall();
		}

		//stuff goes here

		currentFrame++;
	}
	return result;
}


//===============ATTACKS===========================

//0 idle, 1 jumping, 2-jab, 3-fTilt, 4-dTilt, 5-uTilt, 10-hit

Transform Character::jab()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JAB) {
		interuptable = false;
		action = ACTION_JAB;
		activeFrames = 35;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	if (action == ACTION_JAB && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 6) {
			float _kb = 5.5f + (6.55f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.1f, 3.1f, 0.1f), 2.7f, _kb, 55, 7, 0, glm::vec3(((-0.5f + (int)facingRight)*1.1f), 0.0f, 0.0f));
			newAtk->facingRight = facingRight;
			activeHitboxes.push_back(newAtk);
		}
		else if(currentFrame == activeFrames){
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		
		}
		//result.RotateX((5 - abs(currentFrame - 4.0f))*1.5f);
		currentFrame++;
	}
	return result;
}

Transform Character::sAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_SIDE_ATTACK) {
		interuptable = false;
		action = ACTION_SIDE_ATTACK;
		activeFrames = 33;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	if (action == ACTION_SIDE_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 7) {
			float _kb = 8.0f + (9.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			//float _kb = 6.0f + (9.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.8f, _kb, 30, 7, 0, glm::vec3(0,0,0));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			if (facingRight) {
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30, 0.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 3.0f, 0.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 3, 3.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -20, 4.0f, 0));
			}
			else {
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30, 4.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 3.0f, 3.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 3, 0.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -20, 0.0f, 0));
			}
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}
		//result.RotateX((8 - abs(currentFrame - 7.0f))*1.5f);
		//if (facingRight == true)
			//result.GetRot()->y = 1.57f;
		//else
			//result.GetRot()->y = 4.71f;
		currentFrame++;
	}
	return result;
}

Transform Character::dAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_DOWN_ATTACK) {
		interuptable = false;
		action = ACTION_DOWN_ATTACK;
		activeFrames = 33;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	if (action == ACTION_DOWN_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 6) {
			float _kb = 13.0f + (9.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
	//old		//float _kb = 7.0f + (7.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.7f, 0.1f), 2.8f, _kb, 80, 7, 0, glm::vec3((-0.5f + (int)facingRight)*1.0f, 0.0f, 0.0f));
			newAtk->facingRight = facingRight;
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		//result.Scale(vec3(1.0f, 1.0f - (7 - abs(currentFrame - 6.0f))*0.03f,1.0f));
		currentFrame++;
	}
	return result;
}

Transform Character::uAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_UP_ATTACK) {
		interuptable = false;
		action = ACTION_UP_ATTACK;
		activeFrames = 55;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	if (action == ACTION_UP_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 15) {
			float _kb = 13.0f + (7.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			//float _kb = 7.0f + (7.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*2.3f, 1.25f, 0.05f), 3.2f, _kb, 89, 8, 0, glm::vec3((-0.5f + (int)facingRight)*-0.8f, 2.0f, 0.0f));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 4.9f, -45.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 5.9f, 2.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -5.9f, 2.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -4.9f, -45.0f, 0));
			
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		//result.Scale(vec3(1.0f, 1.0f - (15 - abs(currentFrame - 13.0f))*0.03f, 1.0f));
		//result.RotateY(currentFrame * 2);
		currentFrame++;
	}
	return result;
}

//0 idle, 1 jumping, 6-nAir, 7-fAir, 8-dAir, 9-uAir, 10-hit

Transform Character::nAir()
{
	Transform result;
	if (action != ACTION_NEUTRAL_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_NEUTRAL_AERIAL;
		activeFrames = 27;
		currentFrame = 1;
	}
	if (action == ACTION_NEUTRAL_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 5) {
			float _kb = 5.5f + (7.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			//Hitbox *newAtk = new Hitbox(vec3(0.0f, 2.2f, 0.05f)*2.0f, 2.7f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//top
			//activeHitboxes.push_back(newAtk);
			//Hitbox *newAtk2 = new Hitbox(vec3(0.0f, 0.1f, 0.05f)*2.0f, 2.7f, _kb, 80, 9, 0,vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//bottom
			//activeHitboxes.push_back(newAtk2);
			//Hitbox *newAtk3 = new Hitbox(vec3(0.4f, 1.1f, 0.05f)*2.0f, 2.7f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//right
			//activeHitboxes.push_back(newAtk3);
			//Hitbox *newAtk4 = new Hitbox(vec3(-0.4f, 1.1f, 0.05f)*2.0f, 2.7f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//left
			//activeHitboxes.push_back(newAtk4);
			Hitbox *newAtk = new Hitbox(glm::vec3(0, 2.8f, 0.5f), 2.8f, _kb, 75, 7, 0, glm::vec3((-0.5f + (int)facingRight)*0.7f, 0, 0.0f));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -25, 12.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 1, 3.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 1, 3.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -25, 12.0f, 0));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}
		currentFrame++;
		//glm::rotate(result, (float)currentFrame * 10, glm::vec3(0.0f, 1.0f, 0.0f));
		//result.RotateY(currentFrame * 10.0f);
	}
	return result;
}

Transform Character::sAir()
{
	Transform result;
	if (action != ACTION_SIDE_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_SIDE_AERIAL;
		activeFrames = 32;
		currentFrame = 1;
	}
	if (action == ACTION_SIDE_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 7) {
			float _kb = 8.0f + (8.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.8f, _kb, 60, 7, 0, glm::vec3(0, 0, 0));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			if (facingRight) {
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -40, 2.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -2.0f, 2.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 1, 5.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30, 5.5f, 0));
			}
			else {
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -40, 5.5f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -2.0f, 5.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 1, 2.0f, 0));
				newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30, 2.5f, 0));
			}
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}
		//glm::rotate(result, (8.0f - abs(currentFrame - 7.0f)) * 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		//result.RotateX((8 - abs(currentFrame - 7.0f))*1.0f);
		//if (facingRight == true)
			//result.GetRot()->y = 1.57f;
		//else
			//result.GetRot()->y = 4.71f;
		currentFrame++;
	}
	return result;
}

Transform Character::dAir()
{
	Transform result;
	if (action != ACTION_DOWN_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_DOWN_AERIAL;
		activeFrames = 45;
		currentFrame = 1;
	}
	if (action == ACTION_DOWN_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 10) {
			float _kb = 8.0f + (7.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3(0.0f, 0.5f, 0.05f), 3.5f, _kb, 270, 5, 0, glm::vec3(0, 0, 0.0f));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 4, 50.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2, 5.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2, 5.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -4, 50.0f, 0));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}

		//result.GetScale()->y = 1.0f + (7 - abs(currentFrame - 6.0f))*0.02f;
		currentFrame++;
	}
	return result;
}
Transform Character::uAir()
{
	Transform result;
	if (action != ACTION_UP_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_UP_AERIAL;
		activeFrames = 25;
		currentFrame = 1;
	}
	if (action == ACTION_UP_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 5) {
			float _kb = 7.3f + (8.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*2.3f, 1.25f, 0.05f), 2.9f, _kb, 91, 7, 0, glm::vec3(0,0,0));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -0.9f, -45.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -0.9f, 1.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 0.9f, 1.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 0.9f, -35.0f, 0));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}

		//result.GetScale()->y = 1.0f + (7 - abs(currentFrame - 6.0f))*0.02f;
		currentFrame++;
	}
	return result;
}


//0 idle, 1 jumping, 10-hit, 11-nSpecial, 12-sSpecial, 13-dSpecial, 14-uSpecial

Transform Character::nSpecial(bool charging)
{
	//CHARGABLE SPECIAL
	int maxCharge = 170;
	int endlag = 33;
	int startLag = 10;

	Transform result;
	if (interuptable == true && action != ACTION_NEUTRAL_SPECIAL && comboMeter >= 5) {//called on first frame of move press
		interuptable = false;
		action = ACTION_NEUTRAL_SPECIAL;
		activeFrames = maxCharge;//max charge is 60 frames
		currentFrame = 1;

		//MeterCost
		comboMeter -= 5;
	}
	if (action == ACTION_NEUTRAL_SPECIAL && currentFrame <= activeFrames) {

		//if charge released early
		if (!charging && currentFrame > (unsigned int)startLag && activeFrames == maxCharge) {
			//create hitbox
			float _kb = 11.0f + (10.0f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.1f, 2.9f, 0.1f), 3.1f, _kb, (float)angle, 7, 0, glm::vec3((-0.5f + (int)facingRight)*2.0f, -0.22f, 0.0f));
			newAtk->spline = true;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -20.0f, 4.5f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2.0f, 4.0f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2.0f, 0.0f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30.0f, 0.5f, 0.0f));
			newAtk->facingRight = facingRight;
			activeHitboxes.push_back(newAtk);
			//start endlag
			activeFrames = endlag;
			currentFrame = 1;
			aniTimer = 0;
			index = 7;
		}
		//if still charging
		else if (charging && currentFrame > (unsigned int)startLag && activeFrames == (unsigned int)maxCharge) {
			comboMeter--;
			if (index > 7) {
				index = 7;
				aniTimer = 0;
			}
		}
		//max charge move
		if (currentFrame >= activeFrames && activeFrames == maxCharge) {
			//create hitbox
			float _kb = 20.5f + (15.0f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 2.2f, 0.1f), 2.6f, _kb, (float)angle, 7, 0, glm::vec3((-0.5f + (int)facingRight)*2.0f, -0.4f, 0.0f));
			newAtk->spline = true;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -20.0f, 4.5f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2.0f, 4.0f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2.0f, 0.0f, 0.0f));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -30.0f, 0.5f, 0.0f));
			newAtk->facingRight = facingRight;
			activeHitboxes.push_back(newAtk);
			//start endlag
			activeFrames = endlag;
			currentFrame = 1;
			aniTimer = 0;
			index = 7;
			//comboMeter is reset
			comboMeter = 0;
		}
		//done
		else if (currentFrame >= activeFrames && activeFrames == endlag) {//actually done endlag
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}

		//animation every frame
		//glm::rotate(result, (float)currentFrame * 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//result.RotateY( currentFrame * 1.0f);

		currentFrame++;
	}
	return result;
}
Transform Character::sSpecial()
{
	//SIDE SPECIAL 1Press
	Transform result;
	if (interuptable == true && action != ACTION_SIDE_SPECIAL && comboMeter >= 15) {
		interuptable = false;
		action = ACTION_SIDE_SPECIAL;
		activeFrames = 50;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 15;
	}
	if (action == ACTION_SIDE_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if(currentFrame > 7 && currentFrame < 20)
			position.x += (-0.5f + (int)facingRight)*0.7f;
		else if(currentFrame > 3 && currentFrame < 28)
			position.x += (-0.5f + (int)facingRight)*0.3f;

		if (currentFrame == 5) {
			float _kb = 1.5f + (1.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.11, 1.1f, 0.05f)*2.5f, 4.0f, 2.0f * _kb, 45, 15, 0, glm::vec3((-0.5f + (int)facingRight)*0.01, 0.0f, 0.0f));
			newAtk->facingRight = facingRight;

			//Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.1f, _kb, 45, 7, 0, vec3((-0.5f + (int)facingRight)*1.95f, 0.4f, 0.0f));
			//newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.42f, 0, 0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == 28) {
			float _kb = 12.5f + (12.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.5f, 0.6f, 0.1f), 3.8f, _kb, 75, 7, 0, glm::vec3((-0.5f + (int)facingRight)*1.95f, 0.5f, 0.0f));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -25, 0.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2, 0.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2, 4.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -15, 10.5f, 0));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		//glm::rotate(result, (float)currentFrame * 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//result.RotateY(currentFrame * 5.0f);


		currentFrame++;
	}
	return result;
}

Transform Character::dSpecial()
{
	//SIDE SPECIAL 1Press
	Transform result;
	if (interuptable == true && action != ACTION_DOWN_SPECIAL && comboMeter >= 15) {
		interuptable = false;
		action = ACTION_DOWN_SPECIAL;
		activeFrames = 40;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 15;
	}
	if (action == ACTION_DOWN_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 8) {
			float _kb = 11.5f + (11.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3(0.05f, 1.0f, 0.1f), 2.7f, _kb, 75, 5, 0, glm::vec3(0.3f, 0.0f, 0.0f));
			Hitbox *newAtk2 = new Hitbox(glm::vec3(-0.05f, 1.0f, 0.1f), 2.7f, _kb, 75, 5, 0, glm::vec3(-0.3f, 0.0f, 0.0f));
			newAtk->facingRight = facingRight;
			newAtk2->facingRight = !facingRight;

			activeHitboxes.push_back(newAtk);
			activeHitboxes.push_back(newAtk2);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		//glm::rotate(result, (float)currentFrame * 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//result.RotateY(currentFrame * 5.0f);
		currentFrame++;
	}
	return result;
}

Transform Character::uSpecial()
{
	//SIDE SPECIAL 1Press
	Transform result;
	if (interuptable == true && action != ACTION_UP_SPECIAL && comboMeter >= 10) {
		interuptable = false;
		action = ACTION_UP_SPECIAL;
		activeFrames = 36;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 10;
	}
	if (action == ACTION_UP_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame > 6 && currentFrame < 15) {
			//velocity.y = jumpForce * 1.1f;
		}
		if (currentFrame == 8) {
			float _kb = 15.0f + (16.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.7f, 0.1f), 2.9f, _kb, 88, 7, 0, glm::vec3((-0.5f + (int)facingRight)*1.9f, 0.55f, 0.0f));
			newAtk->spline = true;
			newAtk->facingRight = facingRight;
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * -10, -2.5f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2, 2.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 2.5f, 9.0f, 0));
			newAtk->curve.push_back(glm::vec3((-0.5f + (int)facingRight) * 0.5f, 20.5f, 0));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		//glm::rotate(result, (float)currentFrame * 5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		//result.RotateY(currentFrame * 5.0f);
		currentFrame++;
	}
	return result;
}
