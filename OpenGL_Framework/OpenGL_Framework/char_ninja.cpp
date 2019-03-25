#include "char_ninja.h"

//new push
#define BASE_ANI_TOGGLE		true	//non-offensive animations
#define G_ATK_ANI_TOGGLE	true	//ground attacks
#define A_ATK_ANI_TOGGLE	true	//aerials
#define S_ATK_ANI_TOGGLE	true	//specials
#define HITBOX_TOGGLE		false	//visual hitboxes
#define HURTBOX_TOGGLE		false	//visual hurtboxes

Ninja::Ninja(const std::string& bodyName, const std::string& textureName) {
	type = 2;

	if (!(bodyTexture.Load(textureName)))//"./Assets/Textures/Sword.png"))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!(hurtTexture.Load("./Assets/Textures/hurt.png")))//"./Assets/Textures/Sword.png"))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	activeTexture = &bodyTexture;

	aniTimer = 0.f;
	index = 0;
	//======================================================//
		//ANIMATIONS
		///IDLE
	int length = 5;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//9
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/IdlePoses/Idle" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/IdlePoses/Idle" + std::to_string((int)((c + 1) % length))  );

		Mesh* idle = new Mesh();
		idle->LoadFromFile(frame);
		aniFrames[ACTION_IDLE].push_back(idle);
	}
	///WALK
	length = 9;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//14
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/WalkPoses/Walk" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/WalkPoses/Walk" + std::to_string((int)((c + 1) % length))  );

		Mesh* walk = new Mesh();
		walk->LoadFromFile(frame);
		aniFrames[ACTION_WALK].push_back(walk);
	}
	///pre jump
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_PREJUMP].push_back(jab);
	}
	///fall
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string(c));
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string((int)((c + 1) % length)));

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_FALL].push_back(jab);
	}
	///jump
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/JumpPoses/Jump" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_JUMP].push_back(jab);

		Mesh* jab2 = new Mesh();
		jab2->LoadFromFile(frame);
		aniFrames[ACTION_JUMP2].push_back(jab2);
	}
	///hurt
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/HurtPoses/Hurt" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/HurtPoses/Hurt" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_HIT].push_back(jab);
	}
	///initial dash
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/DashPoses/Dash" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/DashPoses/Dash" + std::to_string((int)((c + 1) % length))  );


		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_INTIAL_DASH].push_back(jab);
	}
	///dash
	length = 1;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/DashPoses/Dash" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/DashPoses/Dash" + std::to_string((int)((c + 1) % length))  );


		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_DASH].push_back(jab);
	}
	///run
	length = 5;
	if (BASE_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/RunPoses/Run" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/RunPoses/Run" + std::to_string((int)((c + 1) % length))  );


		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_RUN].push_back(jab);

	}
	//==================================================================//
		//ATTACKS
		///JAB
	length = 6;
	if (G_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//14
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/JabPoses/Jab" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/JabPoses/Jab" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_JAB].push_back(jab);
	}
	///SIDE ATTACK
	length = 5;
	if (G_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//14
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/ForwardTiltPoses/ForwardTilt" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/ForwardTiltPoses/ForwardTilt" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_SIDE_ATTACK].push_back(jab);
	}
	///UP ATTACK
	length = 11;
	if (G_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//14
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/UpTiltPoses/UpTilt" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/UpTiltPoses/UpTilt" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_UP_ATTACK].push_back(jab);
	}
	///DOWN ATTACK
	length = 1;
	if (G_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)//14
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/DownTiltPoses/DownTilt" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/DownTiltPoses/DownTilt" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_DOWN_ATTACK].push_back(jab);
	}
	//==================================================================//
		//AERIALS
		///NeutralAir
	length = 1;
	if (A_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/NeutralAirPoses/NeutralAir" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/NeutralAirPoses/NeutralAir" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_NEUTRAL_AERIAL].push_back(jab);
	}
	///side air
	length = 7;
	if (A_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/ForwardAirPoses/ForwardAir" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/ForwardAirPoses/ForwardAir" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_SIDE_AERIAL].push_back(jab);
	}
	///down aair
	length = 1;
	if (A_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/DownAirPoses/DownAir" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/DownAirPoses/DownAir" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_DOWN_AERIAL].push_back(jab);
	}
	///up air
	length = 9;
	if (A_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/NinjaAnimations/UpAirPoses/UpAir" + std::to_string(c)  );
		frame.push_back("./Assets/Models/NinjaAnimations/UpAirPoses/UpAir" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_UP_AERIAL].push_back(jab);
	}
	//====================================================================//
		//SPECIALS
		///Neutral special
	length = 20;
	if (S_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/KnightAnimations/NeutralSpecialPoses/NeutralSpecial" + std::to_string(c)  );
		frame.push_back("./Assets/Models/KnightAnimations/NeutralSpecialPoses/NeutralSpecial" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_NEUTRAL_SPECIAL].push_back(jab);
	}
	///down special
	length = 13;
	if (S_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/KnightAnimations/DownSpecial/DownSpecial" + std::to_string(c)  );
		frame.push_back("./Assets/Models/KnightAnimations/DownSpecial/DownSpecial" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_DOWN_SPECIAL].push_back(jab);
	}
	///side special
	length = 11;
	if (S_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/KnightAnimations/SideSpecialPoses/SideSpecial" + std::to_string(c)  );
		frame.push_back("./Assets/Models/KnightAnimations/SideSpecialPoses/SideSpecial" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_SIDE_SPECIAL].push_back(jab);
	}
	///up apecial
	length = 8;
	if (S_ATK_ANI_TOGGLE == false)
		length = 1;
	for (int c = 0; c < length; ++c)
	{
		std::vector<std::string> frame;
		frame.push_back("./Assets/Models/KnightAnimations/UpSpecialPoses/UpSpecial" + std::to_string(c)  );
		frame.push_back("./Assets/Models/KnightAnimations/UpSpecialPoses/UpSpecial" + std::to_string((int)((c + 1) % length))  );

		Mesh* jab = new Mesh();
		jab->LoadFromFile(frame);
		aniFrames[ACTION_UP_SPECIAL].push_back(jab);
	}

	//extra
	std::vector<std::string> file;
	file.push_back("./Assets/Models/KnightAnimations/IdlePoses/Idle" + std::to_string(0)  );
	body.LoadFromFile(file);

	//Set Physics
	position = glm::vec3(0, 0, 0);
	lastPos = glm::vec3(0, 0, 0);
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	force = glm::vec3(0, 0, 0);
	facingRight = true;
	ultMode = false;
	//scaling
	scaleX = 1.1f;
	scaleY = 1.15f;
	scaleZ = 1.1f;

	//apply Scale && Rotation
	//glm::scale(transform, glm::vec3(scaleX, scaleY, scaleZ));
	transform.Scale(glm::vec3(scaleX, scaleY, scaleZ));
	//glm::rotate(transform, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	transform.RotateY(90);

	///==============================================================
		//Set Attributes
	///==============================================================

	///Mass
	mass = 9;
	///Gravity Force on Character
	gravity = 0.5f;
	///Multiplier for directional influence while character is in hitstun
	diMultiplier = 0.1f;
	///Multiplier applied to max speed used to decide speed of dash
	dashMultiplier = 1.8f;
	///max run speed
	runSpeed = 0.33f;
	///force applied for running
	runAccel = 0.75f;//0.52f;
	///force appplied for directional movement in air
	airAccel = 0.18f;
	///upwards force for jump
	jumpForce = 0.65f;//0.44f;
	///amount of frames jump last for
	jumpFrames = 8;//12;
	///run InitialDash length (in frames)
	dashLength = 8;
	///number of frames before character leaves ground after jump input
	prejumpLength = 3;
	///total number of air/double jumps
	airJumps = jumpsLeft = 1;
	///amount of frames character is stunned after being launched
	hitstun = 18;//10;
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
	hitBox.push_back("./Assets/Models/Hitbox");
	boxMesh.LoadFromFile(hitBox);
	/*{
		std::cout << "Character Model failed to load.\n";
		system("pause");
		exit(0);
	}*/
	if (!(boxTexture.Load("./Assets/Textures/redclear.png")))
	{
		std::cout << "Knight Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!(shieldTexture.Load("./Assets/Textures/shield.png")))
	{
		std::cout << "Knight Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	comboMeter = 50;

	Hitbox *hurt1 = new Hitbox(glm::vec3(0.0f, 3.0f, 0.0f), 3.0f);
	hurtbox.push_back(hurt1);
	Hitbox *hurt2 = new Hitbox(glm::vec3(0.0f, 1.0f, 0.0f), 3.0f);
	hurtbox.push_back(hurt2);

}

//===============ATTACKS===========================

//0 idle, 1 jumping, 2-jab, 3-fTilt, 4-dTilt, 5-uTilt, 10-hit

Transform Ninja::jab()
{
	Transform result = Transform();
	if (interuptable == true && action != ACTION_JAB) {
		interuptable = false;
		action = ACTION_JAB;
		activeFrames = 18;
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
		else if (currentFrame == activeFrames) {
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

Transform Ninja::sAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_SIDE_ATTACK) {
		interuptable = false;
		action = ACTION_SIDE_ATTACK;
		activeFrames = 17;
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
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*0.2f, 0.6f, 0.1f), 2.8f, _kb, 30, 7, 0, glm::vec3(0, 0, 0));
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

Transform Ninja::dAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_DOWN_ATTACK && downTiltTimer > 50) {
		interuptable = false;
		action = ACTION_DOWN_ATTACK;
		downTiltTimer = 0;
		activeFrames = 33;
		currentFrame = 1;
		aniTimer = 0;
		index = 0;
	}
	if (action == ACTION_DOWN_ATTACK && currentFrame <= activeFrames) {
		if (currentFrame > 6 && currentFrame < 20)
		{
			velocity.x += (-0.5f + (int)facingRight)*0.7f;
		}
		else if (currentFrame > 3 && currentFrame < 28) {


			velocity.x += (-0.5f + (int)facingRight)*0.3f;
		}
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

Transform Ninja::uAttack()
{
	Transform result;
	if (interuptable == true && action != ACTION_UP_ATTACK) {
		interuptable = false;
		action = ACTION_UP_ATTACK;
		activeFrames = 27;
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

Transform Ninja::nAir()
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

Transform Ninja::sAir()
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

Transform Ninja::dAir()
{
	Transform result;
	if (action != ACTION_DOWN_AERIAL || interuptable == true) {
		interuptable = false;
		action = ACTION_DOWN_AERIAL;
		activeFrames = 45;
		currentFrame = 1;
	}
	if (action == ACTION_DOWN_AERIAL && currentFrame <= activeFrames) {
		if (currentFrame > 20 && currentFrame < 38)
		{
			velocity.x += (-0.5f + (int)facingRight)*0.3f;
			velocity.y -= (-0.5f + (int)facingRight)*0.3f;
		}
		else if (currentFrame > 10 && currentFrame < 45) {


			velocity.x += (-0.5f + (int)facingRight)*0.7f;
			velocity.y -= (-0.5f + (int)facingRight)*-0.3f;

		}
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
Transform Ninja::uAir()
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
			Hitbox *newAtk = new Hitbox(glm::vec3((-0.5f + (int)facingRight)*2.3f, 1.25f, 0.05f), 2.9f, _kb, 91, 7, 0, glm::vec3(0, 0, 0));
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

Transform Ninja::nSpecial(bool charging)
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
Transform Ninja::sSpecial()
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

		if (currentFrame > 7 && currentFrame < 20)
			position.x += (-0.5f + (int)facingRight)*0.7f;
		else if (currentFrame > 3 && currentFrame < 28)
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

Transform Ninja::dSpecial()
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

Transform Ninja::uSpecial()
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
