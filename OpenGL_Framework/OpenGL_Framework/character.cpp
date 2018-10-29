#include "character.h"

Character::Character(const std::string& bodyName, const std::string& textureName){
	//Mesh body("./res/monkey3.obj");
	//Shader shader("./res/basicShader");
	//Texture texture("./res/bricks.jpg");

	//Make Characters Model
	//body = new Mesh(bodyName);
	//shader = new Shader(shaderName);
	//texture = new Texture(textureName);

	if (!(body.LoadFromFile(bodyName)))//"./Assets/Models/Sphere.obj"))
	{
		std::cout << "Character Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!(texture.Load(textureName)))//"./Assets/Textures/Sword.png"))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//physics
	position = vec3(0, 0, 0);
	velocity = vec3(0, 0, 0);
	acceleration = vec3(0, 0, 0);
	force = vec3(0, 0, 0);
	mass = 10;
	gravity = 0.2f;
	diMultiplier = 0.5f;

	runspeed = 0.2f;//max speed
	runaccel = 0.3f;//accel multiplyer
	jumpforce = 0.3f;//velocity going up
	jumpframes = 10;//length of jump

	//scaling
	scaleX = 1.1f;
	scaleY = 1.15f;
	scaleZ = 1.1f;

	//transform.Scale(vec3(scaleX, scaleY, scaleZ));
	transform.Scale(vec3(scaleX, scaleY, scaleZ));
	transform.RotateY(90);

	

	//looking
	facingRight = true;

	action = ACTION_FALL;//0 idle, 1 jumping
	idle();
	//debug hitbox
	//boxMesh = new Mesh("./res/transparentsphere.obj");
	//boxShader = new Shader("./res/blueclear");
	//boxTexture = new Texture("./res/redclear.png");
	if (!(boxMesh.LoadFromFile("./Assets/Models/Hitbox.obj")))
	{
		std::cout << "Character Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!(boxTexture.Load("./Assets/Textures/redclear.png")))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	//combo stuff
	comboCount = 0;//counts hits in a row, resets after x time
	comboMeter = 0;//total value of combo meter from 0-100
	comboTimer = 0;//counts time since last hit in combo
	comboMaxTime = 60;//2 seconds times 60fps
	//comboHitMult = 0.1f;//multipy combo meter value by this to get knockback
}

void Character::update(int t, std::vector<bool> inputs) {

	if (comboMeter < 0) {
		comboMeter = 0;
	}

	//COMBO METER
	comboTick();

	//actual update

	//if (action != 10) {
	transform = atkInputHandler(inputs);
	//transform = mat4();
//}
//else {
//	transform = mat4();
//}


//If in Hitstun reduce directional influence
	if (action == ACTION_HIT)
		force.x *= diMultiplier;
	//If in Hitstun reduce directional influence
	if (action == ACTION_FALL || action == ACTION_JUMP || action == ACTION_JUMP2)
		force.x *= 0.3f;

	//physics update
	force = vec3(force.x, 0 - gravity, 0);
	acceleration = force / mass;
	velocity = velocity + (acceleration);
	/*
	//jumping
	if ((((int)inputs[0] - (int)inputs[2]) > 0 || inputs[6]) && interuptable == true && action != 1) {
		interuptable = false;
		action = 1;
		activeFrames = jumpframes;
		currentFrame = 1;
	}
	if (action == 1 && currentFrame <= activeFrames) {
		velocity.y = jumpforce;
		currentFrame++;
	}
	//hit velocity
	if (action == 10 && currentFrame <= activeFrames) {
		velocity = hitforce;
		currentFrame++;
	}
	else if(action == 10){
		interuptable = true;
	}*/


	//quick turnaround code
	///allows "dashdancing" to feel less slow and sluggish
	//if (velocity.x > 0 && ((int)inputs[1] - (int)inputs[3]) < 0 && action == 0)
	//	velocity.x = velocity.x * 0.5f; //-0.2
	//else if (velocity.x < 0 && ((int)inputs[1] - (int)inputs[3]) > 0 && action == 0)
	//	velocity.x = velocity.x * 0.5f;

	//max speed
	float mag = velocity.x;
	if (velocity.x > runspeed)
		velocity.x = runspeed;
	if (velocity.x < (0 - runspeed))
		velocity.x = (0 - runspeed);

	if (position.y <= 1.2f && ((int)inputs[1] - (int)inputs[3]) == 0) {
		velocity.x = velocity.x * 0.9f;
	}
	if (position.y > 1.2f && ((int)inputs[1] - (int)inputs[3]) == 0) {
		velocity.x = velocity.x * 0.99f;
	}

	//Update Position
	position = position + (velocity);

	if (action == ACTION_IDLE || action == ACTION_FALL) {
		///check the way it should face
		if ((int)inputs[1] - (int)inputs[3] > 0) {
			facingRight = true;
		}
		else if ((int)inputs[1] - (int)inputs[3] < 0) {
			facingRight = false;
		}
	}

	///Rotate the player to the correct way they should look
	/*if ((int)inputs[0] - (int)inputs[2] > 0)
		transform.GetRot()->y = 3.14f;
	else */
	if (facingRight == true)
		transform.RotateY(90);
	else
		transform.RotateY(270);

	//Fake Floor Code
	///Will be changed later
	if (position.y <= 1.2f) {
		position.y = 1.2f;
		if (currentFrame >= activeFrames && (action == ACTION_FALL || action == ACTION_HIT)) {
			idle();
		}
	}
	else {
		if (currentFrame >= activeFrames && action == ACTION_HIT) {
			fall();
		}
	}
	
	//Testing Code for Spawning Hitboxes
	//Check Hitboxes
	for (int i = 0; i < activeHitboxes.size(); i++) {
		activeHitboxes[i]->update(t);
		//mat4 move;
		//move.SetTranslation((activeHitboxes[i]->getPosition()));
		//move.Scale(activeHitboxes[i]->getSize());
		//draw
		//boxTexture->Bind();
		//boxShader->Update(move);
		//boxMesh->Draw();

		if (activeHitboxes[i]->isDone())
			activeHitboxes.erase(activeHitboxes.begin() + i);
	}

	transform.SetTranslation(vec3(position.x, position.y, position.z));
	transform.Scale(vec3(scaleX, scaleY, scaleZ));
	//draw
	//shader->Bind();
	//texture->Bind();
	//shader->Update(transform, cam);
	//body->Draw();
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

	for (int i = 0; i < activeHitboxes.size(); i++) {
		int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, activeHitboxes[i]->getTransform().data);

		boxTexture.Bind();
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
		hitforce = add;
		action = ACTION_HIT;
		interuptable = false;
		activeFrames = 10 + hitBy->getKnockback();
		currentFrame = 1;
}

///0-up, 1-left, 2-down, 3-right, 4-A, 5-B

mat4 Character::atkInputHandler(std::vector<bool> inputs)
{
	mat4 result;
	///hit
	if (action == ACTION_HIT) {
		//todo
		velocity = hitforce;
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
	}
	///jump2
	else if ((inputs[6] && action == ACTION_FALL) || action == ACTION_JUMP2) {
		result = jump2();
	}
	///fall
	else if (action == ACTION_FALL) {
		result = fall();
	}
	///prejump
	else if ((inputs[6] && (action == ACTION_IDLE || action == ACTION_RUN || action == ACTION_WALK)) || action == ACTION_PREJUMP) {
		result = prejump();
	}
	//GROUNDED
	else if ((inputs[2] && inputs[4]) || action == ACTION_DOWN_ATTACK) {//down & A = Dtilt
		result = dAttack();
	}
	else if (((inputs[1] || inputs[3]) && inputs[4]) || action == ACTION_SIDE_ATTACK) {//left or right & A = Ftilt
		result = sAttack();
	}
	else if ((inputs[4]) || action == ACTION_JAB) {//just A = jab
		result = jab();
	}
	//SPECIALS
	else if (((inputs[1] || inputs[3]) && inputs[5]) || action == ACTION_SIDE_SPECIAL) {//left or right + B = side Special
		result = sSpecial();
	}
	else if ((inputs[5]) || action == ACTION_NEUTRAL_SPECIAL) {//just B = Neutral Special
		result = nSpecial(inputs[5]);
	}
	//NON-OFFENSIVE
	///dash
	else if ((((inputs[1] || inputs[3]) && action == ACTION_IDLE) || action == ACTION_DASH) && action != ACTION_RUN) {
		result = dash();
	}
	///run
	else if ((inputs[1] || inputs[3]) && (action == ACTION_WALK || action == ACTION_RUN)) {
		result = run();
	}
	///walk
	else if ((inputs[1] || inputs[3]) && (action == ACTION_WALK || action == ACTION_IDLE)) {
		result = walk();
	}
	///idle
	else if (!(inputs[1] || inputs[3]) || action == ACTION_IDLE) {
		result = idle();
	}

	return result;

}

/*

order & equation

air attacks
grounded attacks
specials
jumpsquat
jumping
falling
dash
running
standing

*/

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

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			action = ACTION_PLACEHOLDER;
			return idle();
		}

		//stuff goes here

		
		currentFrame++;
	}
	return result;
}

mat4 Character::walk()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_WALK) {
		action = ACTION_WALK;
		activeFrames = 30;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_WALK && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			return idle();
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;
		force.x = direction*runaccel;
		
		currentFrame++;
	}
	return result;
}

mat4 Character::run()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_RUN) {
		action = ACTION_RUN;
		activeFrames = 30;
		currentFrame = 1;
		interuptable = true;
	}
	else if (action == ACTION_RUN && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			return idle();
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;
		force.x = direction * runaccel;

		currentFrame++;
	}
	return result;
}

mat4 Character::dash()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_DASH) {
		action = ACTION_DASH;
		activeFrames = 10;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_DASH && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			return run();
		}

		//stuff goes here
		int direction = (int)facingRight;
		if (facingRight == 0)
			direction = -1;
		force.x = direction * runaccel;

		currentFrame++;
	}
	return result;
}

mat4 Character::prejump()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_PREJUMP) {
		action = ACTION_PREJUMP;
		activeFrames = 6;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_PREJUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
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
		activeFrames = jumpframes;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_JUMP && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpforce;

		currentFrame++;
	}
	return result;
}

mat4 Character::jump2()
{
	mat4 result = mat4();
	if (interuptable == true && action != ACTION_JUMP2) {
		action = ACTION_JUMP2;
		activeFrames = jumpframes;
		currentFrame = 1;
		interuptable = false;
	}
	else if (action == ACTION_JUMP2 && currentFrame <= activeFrames) {

		if (currentFrame == activeFrames) {
			//if action over, goto idle
			interuptable = true;
			return fall();
		}

		//stuff goes here
		velocity.y = jumpforce;

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

//0 idle, 1 jumping, 2-jab, 3-fTilt, 4-dTilt, 5-uTilt, 10-hit

mat4 Character::jab()
{
	mat4 result = mat4();
	if (interuptable == true && action != 1) {
		interuptable = false;
		action = 2;
		activeFrames = 11;
		currentFrame = 1;
	}
	if (action == 2 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 3) {
			float _kb = 0.65f + (2.35f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3((-0.5f + (int)facingRight), 1.8f, 0.1f), 1.4f, 2.0f * _kb, 60, 5, 0, vec3((-0.5f + (int)facingRight)*0.3f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if(currentFrame == activeFrames){
			action = 0;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
		
		}
		result.RotateX((5 - abs(currentFrame - 4.0f))*1.5f);
		currentFrame++;
	}
	return result;
}

mat4 Character::sAttack()
{
	mat4 result;
	if (interuptable == true && action != 1) {
		interuptable = false;
		action = 3;
		activeFrames = 17;
		currentFrame = 1;
	}
	if (action == 3 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 6) {
			float _kb = 1.25f + (4.25f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3((-0.5f + (int)facingRight), 0.9f, 0.05f)*2.0f, 1.7f, 2.0f * _kb, 30, 7, 0, vec3((velocity.x)*1.8f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 0;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
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
	if (interuptable == true && action != 1) {
		interuptable = false;
		action = 4;
		activeFrames = 15;
		currentFrame = 1;
	}
	if (action == 4 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 3) {
			float _kb = 1.25f + (2.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3((-0.5f + (int)facingRight), 0.4f, 0.1f), 1.4f, 2.0f * _kb, 85, 4, 0, vec3((-0.5f + (int)facingRight)*0.4f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 0;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;

		}
		result.Scale(vec3(1.0f, 1.0f - (7 - abs(currentFrame - 6.0f))*0.03f,1.0f));
		currentFrame++;
	}
	return result;
}

mat4 Character::uAttack()
{
	return mat4();
}

//0 idle, 1 jumping, 6-nAir, 7-fAir, 8-dAir, 9-uAir, 10-hit

mat4 Character::nAir()
{
	mat4 result;
	if (action == 1 || interuptable == true) {
		interuptable = false;
		action = 6;
		activeFrames = 20;
		currentFrame = 1;
	}
	if (action == 6 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 5) {
			float _kb = 1.0f + (3.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3(0.0f, 1.1f, 0.05f)*2.0f, 1.8f, 2.0f * _kb, 70, 9, 0, vec3(velocity.x, velocity.y, 0.0f));//top
			activeHitboxes.push_back(newAtk);
			Hitbox *newAtk2 = new Hitbox(position + vec3(0.0f, 0.0f, 0.05f)*2.0f, 1.8f, 2.0f * _kb, 70, 9, 0,vec3(velocity.x, velocity.y, 0.0f));//bottom
			activeHitboxes.push_back(newAtk2);
			Hitbox *newAtk3 = new Hitbox(position + vec3(0.4f, 0.6f, 0.05f)*2.0f, 1.8f, 2.0f * _kb, 70, 9, 0, vec3(velocity.x, velocity.y, 0.0f));//right
			activeHitboxes.push_back(newAtk3);
			Hitbox *newAtk4 = new Hitbox(position + vec3(-0.4f, 0.6f, 0.05f)*2.0f, 1.8f, 2.0f * _kb, 70, 9, 0, vec3(velocity.x, velocity.y, 0.0f));//left
			activeHitboxes.push_back(newAtk4);
		}
		else if (currentFrame == activeFrames) {
			action = 1;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
		}
		currentFrame++;
		result.RotateY(currentFrame * 10);
	}
	return result;
}

mat4 Character::sAir()
{
	mat4 result;
	if (action == 1 || interuptable == true) {
		interuptable = false;
		action = 7;
		activeFrames = 20;
		currentFrame = 1;
	}
	if (action == 7 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 5) {
			float _kb = 1.5f + (3.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3((-0.5f + (int)facingRight), 2.2f, 0.05f), 1.8f, 2.0f * _kb, 65, 6, 0, vec3((velocity.x)*2.0f, velocity.y, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 1;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
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
	if (action == 1 || interuptable == true) {
		interuptable = false;
		action = 8;
		activeFrames = 25;
		currentFrame = 1;
	}
	if (action == 8 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 10) {
			float _kb = 1.5f + (3.0f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3(0.0f, 0.2f, 0.05f), 2.0f, 2.0f * _kb, 270, 5, 0, vec3((velocity.x), -0.2f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 1;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
		}

		//result.GetScale()->y = 1.0f + (7 - abs(currentFrame - 6.0f))*0.02f;
		currentFrame++;
	}
	return result;
}
mat4 Character::uAir()
{
	mat4 result;
	if (action == 1 || interuptable == true) {
		interuptable = false;
		action = 9;
		activeFrames = 20;
		currentFrame = 1;
	}
	if (action == 9 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future
		if (currentFrame == 6) {
			float _kb = 0.9f + (3.1f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3(0.0f, 1.8f, 0.05f), 2.0f, 3.0f, 90, 5, 0, vec3((velocity.x), 0.3f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 1;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
		}

		//result.GetScale()->y = 1.0f + (7 - abs(currentFrame - 6.0f))*0.02f;
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
	if (interuptable == true && action != 1 && comboMeter >= 5) {//called on first frame of move press
		interuptable = false;
		action = 11;
		activeFrames = maxCharge;//max charge is 60 frames
		currentFrame = 1;

		//MeterCost
		comboMeter -= 5;
	}
	if (action == 11 && currentFrame <= activeFrames) {

		//if charge released early
		if (!charging && currentFrame > startLag && activeFrames == maxCharge) {
			//create hitbox
			float _kb = 2.5f + (3.5f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(position + vec3(-0.5f + (int)facingRight, 0.9f, 0.05f)*2.0f, 2.0f, 2.0f * _kb, angle, 7, 0, vec3((velocity.x)*2.0f, 0.0f, 0.0f));
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
			float _kb = 10.5f + (3.5f * (currentFrame * 0.01f)); //baseKB + (KBgrowth * meter/100)
			unsigned int angle = 45;
			Hitbox *newAtk = new Hitbox(position + vec3(-0.5f + (int)facingRight, 0.9f, 0.05f)*2.0f, 3.0f, 2.0f * _kb, angle, 7, 0, vec3((velocity.x)*2.0f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
			//start endlag
			activeFrames = endlag;
			currentFrame = 1;
			//comboMeter is reset
			comboMeter = 0;
		}
		//done
		else if (currentFrame >= activeFrames && activeFrames == endlag) {//actually done endlag
			//set back to standing
			action = 0;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;
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
	if (interuptable == true && action != 1 && comboMeter >= 20) {
		interuptable = false;
		action = 12;
		activeFrames = 35;
		currentFrame = 1;

		//MeterCost
		comboMeter -= 20;
	}
	if (action == 12 && currentFrame <= activeFrames) {
		//Testing Code for Spawning Hitboxes
		///Will be changed in the future

		if (currentFrame == 20) {
			float _kb = 2.5f + (3.5f * (comboMeter * 0.01f)); //baseKB + (KBgrowth * meter/100)
			Hitbox *newAtk = new Hitbox(position + vec3(-0.5f + (int)facingRight, 0.9f, 0.05f)*2.0f, 3.0f, 2.0f * _kb, 45, 7, 0, vec3((velocity.x)*2.0f, 0.0f, 0.0f));
			activeHitboxes.push_back(newAtk);
		}
		else if (currentFrame == activeFrames) {
			action = 0;
			interuptable = true;
			activeFrames = 0;
			currentFrame = 0;

		}
		result.RotateY(currentFrame * 5);
		position.x += (-0.5f + (int)facingRight)*0.5f;


		currentFrame++;
	}
	return result;
}

mat4 Character::dSpecial()
{
	return mat4();
}

mat4 Character::uSpecial()
{
	return mat4();
}
