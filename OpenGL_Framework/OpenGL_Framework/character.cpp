#include "character.h"

Character::Character(const std::string& bodyName, const std::string& textureName){

	/*//Load Model Texture
	if (!(body.LoadFromFile(bodyName)))//"./Assets/Models/Sphere.obj"))
	{
		std::cout << "Character Model failed to load.\n";
		system("pause");
		exit(0);
	}*/
	std::vector<std::string> knightFiles;
	for (int c = 0; c < 2; ++c)
	{
		knightFiles.push_back("./Assets/Models/KnightIdle" + std::to_string(c) + ".obj");
		//marioFiles.push_back("../assets/models/Mario_Export" + std::to_string(c) + ".bin");
	}
	body.LoadFromFile(knightFiles);

	if (!(texture.Load(textureName)))//"./Assets/Textures/Sword.png"))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//Set Physics
	position = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);
	acceleration = vec3(0, 0, 0);
	force = vec3(0, 0, 0);
	facingRight = true;
	blocking = false;
	blockSuccessful = false;
	//scaling
	scaleX = 1.1f;
	scaleY = 1.15f;
	scaleZ = 1.1f;

	//apply Scale && Rotation
	transform.Scale(vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(90);

///==============================================================

	//Set Attributes
	///-------------
	///Mass
	mass = 9;
	///Gravity Force on Character
	gravity = 0.36f;
	///Multiplier for directional influence while character is in hitstun
	diMultiplier = 0.1f;
	///max run speed
	runSpeed = 0.23f;
	///force applied for running
	runAccel = 0.35f;
	///force appplied for directional movement in air
	airAccel = 0.15f;
	///upwards force for jump
	jumpForce = 0.3f;
	///amount of frames jump last for
	jumpFrames = 12;
	///Dash length (in frames)
	dashLength = 18;
	///number of frames before character leaves ground after jump input
	prejumpLength =3;
	///total number of air/double jumps
	airJumps = jumpsLeft = 1;
	///amount of frames character is stunned after being launched
	hitstun = 10;
	///amount of frames character is launched for when hit
	hitframes = 12;

	//set combo stuff
	comboCount = 0;
	comboMeter = 0;
	comboTimer = 0;
	comboMaxTime = 120;//1 seconds times 60fps

	//Set Starting Action
	action = ACTION_FALL;//0 idle, 1 jumping
	idle();


	std::vector<std::string> hitBox;
	hitBox.push_back("./Assets/Models/Hitbox.obj");
	boxMesh.LoadFromFile(hitBox);
	/*{
		std::cout << "Character Model failed to load.\n";
		system("pause");
		exit(0);
	}*/
	if (!(boxTexture.Load("./Assets/Textures/redclear.png")))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!(shieldTexture.Load("./Assets/Textures/shield.png")))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
}

void Character::update(int t, std::vector<bool> inputs) {

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

	//actual update
	force.x = 0;
	transform = atkInputHandler(inputs);

	//physics update
	force = vec3(force.x, 0 - gravity, 0);
	acceleration = force / mass;
	velocity = velocity + (acceleration);

	//max speed
	float mag = velocity.x;
	if (velocity.x > runSpeed)
		velocity.x = runSpeed;
	if (velocity.x < (0 - runSpeed))
		velocity.x = (0 - runSpeed);

	//friction
	if (position.y <= 0.0f && (((int)inputs[1] - (int)inputs[3]) == 0 || (action != ACTION_WALK && action != ACTION_RUN && action != ACTION_INTIAL_DASH && action != ACTION_PREJUMP))) {
		velocity.x = velocity.x * 0.7f;
	}
	if (position.y > 0.0f && !inputs[1] && !inputs[3]) {
		velocity.x = velocity.x * 0.9f;
	}

	//Update Position
	position = position + (velocity);


	///Rotate the player to the correct way they should look
	if (facingRight == true)
		transform.RotateY(0);
	else
		transform.RotateY(180);

	//Fake Floor Code
	///Will be changed later
	if (position.y < 0.0f) {
		//called on landing
		position.y = 0.0f;
		jumpsLeft = airJumps;
		if (action == ACTION_HIT) {
			velocity.y *= -0.75f;
			velocity.x *= 0.5f;
			hitForce.y *= -0.9f;
		}
		else if ((currentFrame >= activeFrames || interuptable == true) && (action == ACTION_FALL)) {
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			idle();
			velocity.x *= 0.3f;
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
	for (int i = 0; i < activeHitboxes.size(); i++) {
		activeHitboxes[i]->update(t, position);

		if (activeHitboxes[i]->isDone())
			activeHitboxes.erase(activeHitboxes.begin() + i);
	}

	transform.SetTranslation(vec3(position.x, position.y, position.z));
	transform.Scale(vec3(scaleX, scaleY, scaleZ));
}

//Returns the Players Position
vec3 Character::getPosition()
{
	return position;
}

void Character::draw(ShaderProgram GBufferPass) {

	int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
	glUniformMatrix4fv(modelLoc, 1, false, transform.data);

	texture.Bind();
	glBindVertexArray(body.VAO);

	// Adjust model matrix for next object's location
	glDrawArrays(GL_TRIANGLES, 0, body.GetNumVertices());
	glUniformMatrix4fv(modelLoc, 1, false, mat4().data);

}


void Character::drawBoxes(ShaderProgram GBufferPass) {

	for (int i = 0; i < activeHitboxes.size(); i++) {
		int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, activeHitboxes[i]->getTransform().data);

		boxTexture.Bind();
		glBindVertexArray(boxMesh.VAO);

		// Adjust model matrix for next object's location
		glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, mat4().data);
	}

	if (blocking) {
		int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
		mat4 shield;
		int i = (int)facingRight;
		if (i == 0) i = -1;
		shield.SetTranslation(position + vec3(i*0.7f, 1.5f, 0));
		shield.Scale(vec3(1, 5.5f, 5.5));
		glUniformMatrix4fv(modelLoc, 1, false, shield.data);

		shieldTexture.Bind();
		glBindVertexArray(boxMesh.VAO);

		// Adjust model matrix for next object's location
		glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, mat4().data);
	}
}

void Character::drawShadow(ShaderProgram GBufferPass)
{
	//GBufferPass.SendUniformMat4("uProj", transform.data, true);

	glBindVertexArray(body.VAO);
	glDrawArrays(GL_TRIANGLES, 0, body.GetNumVertices());
	//glUniformMatrix4fv(modelLoc, 1, false, mat4().data);
}

//Sets player position
void Character::setPosition(vec3 pos){
	position = pos;
}

//Returns a list of all active hitboxes from this player
std::vector<Hitbox*> Character::getHitboxes()
{
	return activeHitboxes;
}

//called on hit
void Character::hit(Hitbox* hitBy) {
		float result = hitBy->getVelocity().x;
		if (result <= 0)
			result = 0.06f;
		else
			result = -0.06f;

		float xComp = hitBy->getKnockback() * cos((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);
		float yComp = hitBy->getKnockback() * sin((180.0f - hitBy->getAngle()) * 3.14f / 180.0f);//*0.0174772222222222f);

		vec3 add(result * xComp, 0.06f * yComp, 0);
		hitForce = add;
		action = ACTION_HIT;
		interuptable = false;
		activeFrames = hitframes + hitstun + hitBy->getKnockback();//
		currentFrame = 1;
}

///0-up, 1-left, 2-down, 3-right, 4-A, 5-B, 6-jump
///7-hardLeft, 8-hardRight
///9-shield
mat4 Character::atkInputHandler(std::vector<bool> inputs)
{
	mat4 result;
	///hit
	if (action == ACTION_HIT) {
		//todo
		//If in Hitstun reduce directional influence
		force.x = (inputs[1] - inputs[3]) *  diMultiplier;
		if (currentFrame < (hitframes))//only launched for hitframes, character will just be stunned for the remaining frames (hitstun + moves kb)
			velocity = hitForce;
		currentFrame++;
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
	///SHIELD
	else if (((inputs[9] && (action < 10)) || action == ACTION_BLOCK)) {//just B = Neutral Special
		result = block(inputs[9]);
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
	else if ((inputs[2] && inputs[5]) || action == ACTION_DOWN_SPECIAL) {//down + B = down Special
		result = dSpecial();
	}
	else if ((inputs[0] && inputs[5]) || action == ACTION_UP_SPECIAL) {//up + B = up Special
		result = uSpecial();
	}
	else if (((inputs[1] || inputs[3]) && inputs[5]) || action == ACTION_SIDE_SPECIAL) {//left or right + B = side Special
		result = sSpecial();
	}
	else if ((inputs[5]) || action == ACTION_NEUTRAL_SPECIAL) {//just B = Neutral Special
		result = nSpecial(inputs[5]);
	}
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

mat4 Character::idle()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_IDLE) {
		action = ACTION_IDLE;
		activeFrames = 60;
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

mat4 Character::walk(bool held)
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_WALK) {
		action = ACTION_WALK;
		activeFrames = 30;
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

mat4 Character::run(bool held)
{
	mat4 result = mat4();
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

mat4 Character::initialDash(bool right, bool left)
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_INTIAL_DASH) {
		action = ACTION_INTIAL_DASH;
		activeFrames = dashLength;
		currentFrame = 1;
		interuptable = true;
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
		if (facingRight == 0)
			direction = -1;

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

mat4 Character::prejump()
{
	mat4 result = mat4();
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

mat4 Character::jump()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_JUMP) {
		action = ACTION_JUMP;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		velocity.x *= 0.9f;
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

mat4 Character::jump2()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_JUMP2 && jumpsLeft > 0) {
		action = ACTION_JUMP2;
		activeFrames = jumpFrames;
		currentFrame = 1;
		interuptable = false;
		jumpsLeft--;
		velocity.x *= 0.5f;
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

mat4 Character::fall()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_FALL) {
		action = ACTION_FALL;
		activeFrames = 30;
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

mat4 Character::jab()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_JAB) {
		interuptable = false;
		action = ACTION_JAB;
		activeFrames = 17;
		currentFrame = 1;
	}
	if (action == ACTION_JAB && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 5) {
			float _kb = 3.5f + (3.55f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.05f, 1.8f, 0.1f), 1.7f, _kb, 55, 6, 0, vec3((-0.5f + (int)facingRight)*0.5f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if(currentFrame == activeFrames){
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		
		}
		result.RotateX((5 - abs(currentFrame - 4.0f))*1.5f);
		currentFrame++;
	}
	return result;
}

mat4 Character::sAttack()
{
	mat4 result;
	if (interuptable == true && action != ACTION_SIDE_ATTACK) {
		interuptable = false;
		action = ACTION_SIDE_ATTACK;
		activeFrames = 24;
		currentFrame = 1;
	}
	if (action == ACTION_SIDE_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 7) {
			float _kb = 4.0f + (6.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.1f, _kb, 45, 7, 0, vec3((-0.5f + (int)facingRight)*1.95f, 0.4f, 0.0f));
			newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.42f, 0, 0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}
		result.RotateX((8 - abs(currentFrame - 7.0f))*1.5f);
		//if (facingRight == true)
			//result.GetRot()->y = 1.57f;
		//else
			//result.GetRot()->y = 4.71f;
		currentFrame++;
	}
	return result;
}

mat4 Character::dAttack()
{
	mat4 result;
	if (interuptable == true && action != ACTION_DOWN_ATTACK) {
		interuptable = false;
		action = ACTION_DOWN_ATTACK;
		activeFrames = 23;
		currentFrame = 1;
	}
	if (action == ACTION_DOWN_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 6) {
			float _kb = 6.0f + (5.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.5f, 0.1f), 2.2f, _kb, 80, 6, 0, vec3((-0.5f + (int)facingRight)*0.75f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		result.Scale(vec3(1.0f, 1.0f - (7 - abs(currentFrame - 6.0f))*0.03f,1.0f));
		currentFrame++;
	}
	return result;
}

mat4 Character::uAttack()
{
	mat4 result;
	if (interuptable == true && action != ACTION_UP_ATTACK) {
		interuptable = false;
		action = ACTION_UP_ATTACK;
		activeFrames = 30;
		currentFrame = 1;
	}
	if (action == ACTION_UP_ATTACK && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 6) {
			float _kb = 5.0f + (4.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*2.5f, 0.55f, 0.05f), 2.9f, _kb, 89, 6, 0, vec3((-0.5f + (int)facingRight)*-0.8f, 1.66f, 0.0f));
			newAtk->acceleration = vec3(0, -0.46,0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		result.Scale(vec3(1.0f, 1.0f - (15 - abs(currentFrame - 13.0f))*0.03f, 1.0f));
		result.RotateY(currentFrame * 2);
		currentFrame++;
	}
	return result;
}

//0 idle, 1 jumping, 6-nAir, 7-fAir, 8-dAir, 9-uAir, 10-hit

mat4 Character::nAir()
{
	mat4 result;
	if (action != ACTION_NEUTRAL_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_NEUTRAL_AERIAL;
		activeFrames = 22;
		currentFrame = 1;
	}
	if (action == ACTION_NEUTRAL_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 5) {
			float _kb = 3.5f + (4.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3(0.0f, 1.4f, 0.05f)*2.0f, 2.1f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//top
			activeHitboxes.push_back(newAtk);
			Hitbox *newAtk2 = new Hitbox(vec3(0.0f, 0.0f, 0.05f)*2.0f, 2.1f, _kb, 80, 9, 0,vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//bottom
			activeHitboxes.push_back(newAtk2);
			Hitbox *newAtk3 = new Hitbox(vec3(0.4f, 0.7f, 0.05f)*2.0f, 2.1f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//right
			activeHitboxes.push_back(newAtk3);
			Hitbox *newAtk4 = new Hitbox(vec3(-0.4f, 0.7f, 0.05f)*2.0f, 2.1f, _kb, 80, 9, 0, vec3((-0.5f + (int)facingRight)*0.01f, 0, 0.0f));//left
			activeHitboxes.push_back(newAtk4);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}
		currentFrame++;
		result.RotateY(currentFrame * 10);
	}
	return result;
}

mat4 Character::sAir()
{
	mat4 result;
	if (action != ACTION_SIDE_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_SIDE_AERIAL;
		activeFrames = 28;
		currentFrame = 1;
	}
	if (action == ACTION_SIDE_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 5) {
			float _kb = 6.0f + (5.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3(0, 2.2f, 0.05f), 2.2f, _kb, 65, 7, 0, vec3((-0.5f + (int)facingRight)*0.7f, 0, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto fall
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();
		}
		result.RotateX((8 - abs(currentFrame - 7.0f))*1.0f);
		//if (facingRight == true)
			//result.GetRot()->y = 1.57f;
		//else
			//result.GetRot()->y = 4.71f;
		currentFrame++;
	}
	return result;
}

mat4 Character::dAir()
{
	mat4 result;
	if (action != ACTION_DOWN_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_DOWN_AERIAL;
		activeFrames = 27;
		currentFrame = 1;
	}
	if (action == ACTION_DOWN_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 10) {
			float _kb = 6.0f + (4.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3(0.0f, 0.2f, 0.05f), 2.5f, _kb, 270, 5, 0, vec3(0, -0.2f, 0.0f));
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
mat4 Character::uAir()
{
	mat4 result;
	if (action != ACTION_UP_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_UP_AERIAL;
		activeFrames = 30;
		currentFrame = 1;
	}
	if (action == ACTION_UP_AERIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 7) {
			float _kb = 5.3f + (5.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*2.5f, 0.55f, 0.05f), 2.9f, _kb, 91, 7, 0, vec3((-0.5f + (int)facingRight)*-0.7f, 1.66f, 0.0f));
			newAtk->acceleration = vec3(0, -0.46, 0);

			
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

mat4 Character::block(bool held)
{
	int endlag = 8;//endlag after a successful block is this, on failed attempt its doubled
	int startLag = 5;

	mat4 result;
	if (interuptable == true && action != ACTION_BLOCK) {//called on first frame of move press
		interuptable = false;
		action = ACTION_BLOCK;
		activeFrames = 30;
		currentFrame = 1;

	}
	if (action == ACTION_BLOCK) {

		//if released
		if (!held && currentFrame > startLag && (activeFrames != endlag && activeFrames != endlag*2)) {
			//start endlag
			activeFrames = endlag;
			if (!blockSuccessful)
				activeFrames *= 2;
			currentFrame = 1;
		}
		//held
		else if (held && currentFrame <= activeFrames && currentFrame > startLag) {

			interuptable = false;
			currentFrame = startLag + 2;
			//std::cout << " blocking" << std::endl;
			blocking = true;
		}
		//release
		else if ((activeFrames == endlag || activeFrames == endlag * 2) && currentFrame >= activeFrames) {//actually done endlag
			//if action over, goto idle
			blocking = false;
			blockSuccessful = false;
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();
		}
		currentFrame++;
	}
	return result;
}


//0 idle, 1 jumping, 10-hit, 11-nSpecial, 12-sSpecial, 13-dSpecial, 14-uSpecial

mat4 Character::nSpecial(bool charging)
{
	//CHARGABLE SPECIAL
	int maxCharge = 100;
	int endlag = 20;
	int startLag = 10;

	mat4 result;
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
		if (!charging && currentFrame > startLag && activeFrames == maxCharge) {
			//create hitbox
			float _kb = 7.0f + (7.0f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.1f, 2.3f, 0.1f), 2.4f, _kb, angle, 7, 0, vec3((-0.5f + (int)facingRight)*2.0f, -0.22f, 0.0f));
			newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.46f, 0, 0);
			activeHitboxes.push_back(newAtk);
			//start endlag
			activeFrames = endlag;
			currentFrame = 1;
		}
		//if still charging
		else if (charging && currentFrame > startLag && activeFrames < maxCharge) {
			comboMeter--;
		}
		//max charge move
		if (currentFrame >= activeFrames && activeFrames == maxCharge) {
			//create hitbox
			float _kb = 20.5f + (7.0f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 1.4f, 0.1f), 2.3f, _kb, angle, 7, 0, vec3((-0.5f + (int)facingRight)*2.0f, -0.4f, 0.0f));
			newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.42f, 0, 0);
			activeHitboxes.push_back(newAtk);
			//start endlag
			activeFrames = endlag;
			currentFrame = 1;
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
		result.RotateY( currentFrame * 1);

		currentFrame++;
	}
	return result;
}
mat4 Character::sSpecial()
{
	//SIDE SPECIAL 1Press
	mat4 result;
	if (interuptable == true && action != ACTION_SIDE_SPECIAL && comboMeter >= 15) {
		interuptable = false;
		action = ACTION_SIDE_SPECIAL;
		activeFrames = 40;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 15;
	}
	if (action == ACTION_SIDE_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if(currentFrame < 35)
			position.x += (-0.5f + (int)facingRight)*0.3f;

		if (currentFrame == 5) {
			float _kb = 4.5f + (1.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.05, 0.9f, 0.05f)*2.0f, 4.0f, 2.0f * _kb, 15, 15, 0, vec3((-0.5f + (int)facingRight)*0.01, 0.0f, 0.0f));

			//Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.1f, _kb, 45, 7, 0, vec3((-0.5f + (int)facingRight)*1.95f, 0.4f, 0.0f));
			//newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.42f, 0, 0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == 25) {
			float _kb = 5.5f + (4.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.1f, _kb, 75, 7, 0, vec3((-0.5f + (int)facingRight)*1.95f, 0.4f, 0.0f));
			newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.42f, 0, 0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}
		result.RotateY(currentFrame * 5);


		currentFrame++;
	}
	return result;
}

mat4 Character::dSpecial()
{
	//SIDE SPECIAL 1Press
	mat4 result;
	if (interuptable == true && action != ACTION_DOWN_SPECIAL && comboMeter >= 15) {
		interuptable = false;
		action = ACTION_DOWN_SPECIAL;
		activeFrames = 35;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 15;
	}
	if (action == ACTION_DOWN_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 8) {
			float _kb = 6.5f + (6.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3(0.05f, 0.5f, 0.1f), 2.7f, _kb, 75, 5, 0, vec3(0.3f, 0.0f, 0.0f));
			Hitbox *newAtk2 = new Hitbox(vec3(-0.05f, 0.5f, 0.1f), 2.7f, _kb, 75, 5, 0, vec3(-0.3f, 0.0f, 0.0f));

			activeHitboxes.push_back(newAtk);
			activeHitboxes.push_back(newAtk2);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return idle();

		}

		result.RotateY(currentFrame * 5);
		currentFrame++;
	}
	return result;
}

mat4 Character::uSpecial()
{
	//SIDE SPECIAL 1Press
	mat4 result;
	if (interuptable == true && action != ACTION_UP_SPECIAL && comboMeter >= 10) {
		interuptable = false;
		action = ACTION_UP_SPECIAL;
		activeFrames = 20;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 10;
	}
	if (action == ACTION_UP_SPECIAL && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame > 6 && currentFrame < 15) {
			velocity.y = jumpForce * 1.1;
		}
		if (currentFrame == 8) {
			float _kb = 7.5f + (2.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(vec3((-0.5f + (int)facingRight)*0.2f, 0.7f, 0.1f), 2.5f, _kb, 88, 7, 0, vec3((-0.5f + (int)facingRight)*1.85f, 0.45f, 0.0f));
			newAtk->acceleration = vec3((-0.5f + (int)facingRight)*-0.5f, 0, 0);
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			action = ACTION_PLACEHOLDER;
			return fall();

		}

		result.RotateY(currentFrame * 5);
		currentFrame++;
	}
	return result;
}
