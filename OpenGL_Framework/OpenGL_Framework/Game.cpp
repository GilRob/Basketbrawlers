#include "Game.h"
#include "Utilities.h"

Game::Game()
	: GBuffer(3), DeferredComposite(1), ShadowMap(0), /*EdgeMap(1),*/ WorkBuffer1(1), WorkBuffer2(1), HudMap(1)
	//This constructor in the initializer list is to solve the issue of creating a frame buffer object without no default constructor
	//This will occur before the brackets of the constructor starts (Reference at Week #6 video Time: 47:00)
	//The number is the number of color textures
{
}

Game::~Game()
{
	delete updateTimer;

	//StaticGeometry.UnLoad();
	BloomHighPass.UnLoad();
	BlurHorizontal.UnLoad();
	BlurVertical.UnLoad();
	BloomComposite.UnLoad();
	GBufferPass.UnLoad();
	DeferredLighting.UnLoad();
	AniShader.UnLoad();
	HudShader.UnLoad();
	boxMesh.Unload();
	boxTexture.Unload();
	Court.Unload();
	CourtTexture.Unload();
	Background.Unload();
	BackgroundTexture.Unload();
	HudObj.Unload();
	P1Hud.Unload();
	P1Bar.Unload();
	P2Hud.Unload();
	P2Bar.Unload();
}

void Game::initializeGame()
{

	//Only needs to be done once
	glEnable(GL_DEPTH_TEST);
	
	//glutFullScreen();
	InitFullScreenQuad();

	inputs2 = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A

	playerOne = new Character("./Assets/Models/Knight.obj", "./Assets/Textures/player1.png");

	inputs = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A

	playerTwo = new Character("./Assets/Models/Knight.obj", "./Assets/Textures/player2.png");

	if (!HudShader.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/HudShader.frag"))
	{
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!GBufferPass.Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomHighPass.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomHighPass.frag"))
	{
		std::cout << "BHP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BlurHorizontal.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurHorizontal.frag"))
	{
		std::cout << "BH Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BlurVertical.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BlurVertical.frag"))
	{
		std::cout << "BV Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!BloomComposite.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Bloom/BloomComposite.frag"))
	{
		std::cout << "BC Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	/*if (!GBufferPass.Load("./Assets/Shaders/StaticGeometry.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "GBP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}*/

	if (!DeferredLighting.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/DeferredLighting.frag"))
	{
		std::cout << "DL Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!AniShader.Load("./Assets/Shaders/AnimationShader.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "AS Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	std::vector<std::string> court;
	court.push_back("./Assets/Models/KnightCourt.obj");
	Court.LoadFromFile(court);

	if (!CourtTexture.Load("./Assets/Textures/GameCastleTexture.png"))
	{
		std::cout << "Court Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	std::vector<std::string> bkg;
	bkg.push_back("./Assets/Models/Background.obj");
	Background.LoadFromFile(bkg);

	std::vector<std::string> hud1;
	hud1.push_back("./Assets/Models/UI_Object.obj");
	HudObj.LoadFromFile(hud1);

	if (!BackgroundTexture.Load("./Assets/Textures/BackgroundTexture.jpg"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	BGTransform.SetTranslation(glm::vec3(0, 3, -40));

	if (!P1Hud.Load("./Assets/Textures/PlayerOneHud.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!P1Bar.Load("./Assets/Textures/PlayerOneBar.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!P2Hud.Load("./Assets/Textures/PlayerTwoHud.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	if (!P2Bar.Load("./Assets/Textures/PlayerTwoBar.png"))
	{
		std::cout << "BKG Texture failed to load.\n";
		system("pause");
		exit(0);
	}


	std::vector<std::string> hitBox;
	hitBox.push_back("./Assets/Models/Hitbox.obj");
	boxMesh.LoadFromFile(hitBox);

	if (!(boxTexture.Load("./Assets/Textures/redclear.png")))
	{
		std::cout << "Character Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	loadTime();

	GBuffer.InitDepthTexture(WINDOW_WIDTH, WINDOW_HEIGHT);
	//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
	//These parameters can be changed to whatever you want
	GBuffer.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE); //Flat color
	GBuffer.InitColorTexture(1, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB16, GL_NEAREST, GL_CLAMP_TO_EDGE); //Normals (xyz)
	//Buffer explained at Week 10 time: 5:30 - 7:45
	GBuffer.InitColorTexture(2, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB32F, GL_NEAREST, GL_CLAMP_TO_EDGE); //View Space Positions (xyz)
	if (!GBuffer.CheckFBO())
	{
		std::cout << "GB FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	DeferredComposite.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!DeferredComposite.CheckFBO())
	{
		std::cout << "DC FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	ShadowMap.InitDepthTexture(SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	if (!ShadowMap.CheckFBO())
	{
		std::cout << "SM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	//THis is a single channel texture explained at Week 11 time: ~3:30
	/*EdgeMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!EdgeMap.CheckFBO())
	{
		std::cout << "EM FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}*/

	WorkBuffer1.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer1.CheckFBO())
	{
		std::cout << "WB1 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	WorkBuffer2.InitColorTexture(0, WINDOW_WIDTH / (unsigned int)BLOOM_DOWNSCALE, WINDOW_HEIGHT / (unsigned int)BLOOM_DOWNSCALE, GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer2.CheckFBO())
	{
		std::cout << "WB2 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}
	//0 is equal to 1 for the index. To make another color texture it is as easy as changing the list size in the contructor and copying the line below
	//These parameters can be changed to whatever you want
	HudMap.InitColorTexture(0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	if (!HudMap.CheckFBO())
	{
		std::cout << "HudMap FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}


	/*CameraTransform.Translate(vec3(0.0f, 7.5f, 20.0f));
	CameraTransform.RotateX(-15.0f);*/
	CameraProjection = Transform::PerspectiveProjection(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 10000.0f);
	//ShadowProjection.OrthographicProjection(35.0f, -35.0f, 35.0f, -35.0f, -10.0f, 100.0f);
	ShadowProjection = Transform::OrthographicProjection(-35.0f, 35.0f, 35.0f, -35.0f, -25.0f, 100.0f);
	hudProjection = Transform::OrthographicProjection((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * 0.5f, (float)WINDOW_HEIGHT * -0.5f, -10.0f, 100.0f);


	Hitbox *hurt1 = new Hitbox(glm::vec3(25.0f, 12.0f, 0.0f), 6.0f);
	Netbox.push_back(hurt1);
	Hitbox *hurt2 = new Hitbox(glm::vec3(-25.0f, 12.0f, 0.0f), 6.0f);
	Netbox.push_back(hurt2);

	score1 = 0;
	score2 = 0;


	playerOne->setPosition(glm::vec3(-5, 0, 0));
	playerTwo->setPosition(glm::vec3(5, 0, 0));

	updateTimer = new Timer();
}

void Game::update()
{
	// update our clock so we have the delta time since the last update
	updateTimer->tick();

	float deltaTime = updateTimer->getElapsedTimeSeconds();
	TotalGameTime += deltaTime;
	//character collisions
		///Allow Player to pass through one another, but will disallow them to stand in the same spot, will slowly push eachother awayy
	float diffx = (playerOne->getPosition().x) - (playerTwo->getPosition().x);//difference between characters x
	float diffy = (playerOne->getPosition().y) - (playerTwo->getPosition().y);//difference between characters y
	if (abs(diffx) < 0.6f && abs(diffx) != 0 && abs(diffy) < 1.5f) {//if they are touching
		//push them off
		playerOne->setPosition(playerOne->getPosition() + glm::vec3(((diffx / abs(diffx))*0.01f), 0, 0));
		playerTwo->setPosition(playerTwo->getPosition() + glm::vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
	}

	//new hitbox collisions
	for (unsigned int i = 0; i < playerOne->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < playerTwo->getHurtboxes().size(); j++) {

			glm::vec3 diff = playerOne->getHitboxes()[i]->getPosition() - playerTwo->getHurtboxes()[j]->getPosition();
			float size = (playerOne->getHitboxes()[i]->getSize() + playerTwo->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {
				if (playerTwo->blocking && (playerOne->facingRight != playerTwo->facingRight)) {//add only in front condition
					playerTwo->blockSuccessful = true;
					playerOne->getHitboxes()[i]->setDone();
					i = 100;
					j = 100;
				}
				else {
					playerTwo->hit(playerOne->getHitboxes()[i]);
					playerOne->comboAdd();
					playerOne->getHitboxes()[i]->setDone();
					i = 100;
					j = 100;
				}
			}

		}

	}

	
	for (unsigned int i = 0; i < playerTwo->getHitboxes().size(); i++) {
		for (unsigned int j = 0; j < playerOne->getHurtboxes().size(); j++) {

			glm::vec3 diff = playerTwo->getHitboxes()[i]->getPosition() - playerOne->getHurtboxes()[j]->getPosition();
			float size = (playerTwo->getHitboxes()[i]->getSize() + playerOne->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {
				if (playerOne->blocking && (playerOne->facingRight != playerTwo->facingRight)) {//add only in front condition
					playerOne->blockSuccessful = true;
					playerTwo->getHitboxes()[i]->setDone();
					i = 100;
					j = 100;
				}
				else {
					playerOne->hit(playerTwo->getHitboxes()[i]);
					playerTwo->comboAdd();
					playerTwo->getHitboxes()[i]->setDone();
					i = 100;
					j = 100;
				}
			}

		}

	}

	//Check Hurtboxes
	for (unsigned int i = 0; i < Netbox.size(); i++) {
		Netbox[i]->update((int)deltaTime, glm::vec3());
	}

	updateInputs();
	playerOne->update((int)deltaTime, inputs);
	playerTwo->update((int)deltaTime, inputs2);

	////score
	//if (abs(abs(playerOne->getPosition().x) - 26) < 1.3f && abs(playerOne->getPosition().y - 10) < 1.3f) {
	//	playerOne->respawn();
	//	std::cout << std::endl << "Player 2 Scored" << std::endl;
	//}
	//if (abs(abs(playerTwo->getPosition().x) - 26) < 1.3f && abs(playerTwo->getPosition().y - 10) < 1.3f) {
	//	playerTwo->respawn();
	//	std::cout << std::endl << "Player 1 Scored" << std::endl;
	//}

	//new score code
	for (unsigned int i = 0; i < Netbox.size(); i++) {
		for (unsigned int j = 0; j < playerTwo->getHurtboxes().size(); j++) {
			glm::vec3 diff = Netbox[i]->getPosition() - playerTwo->getHurtboxes()[j]->getPosition();
			float size = (Netbox[i]->getSize() + playerTwo->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {
				playerTwo->respawn();
				std::cout << std::endl << "Player 1 Scored" << std::endl;
				score1++;
				//i = 100;
				j = 100;

			}
		}
		for (unsigned int j = 0; j < playerOne->getHurtboxes().size(); j++) {
			glm::vec3 diff = Netbox[i]->getPosition() - playerOne->getHurtboxes()[j]->getPosition();
			float size = (Netbox[i]->getSize() + playerOne->getHurtboxes()[j]->getSize()) *0.5f;
			if (/*diff.Length()*/ glm::length(diff) < size) {
				playerOne->respawn();
				std::cout << std::endl << "Player 2 Scored" << std::endl;
				score2++;
				//i = 100;
				j = 100;

			}
		}
	}

	
	//inputs = { false, false, false, false, false, false }; //up, left, down, right, A, B

	//DYNAMIC CAM
	//camera control
	float dist = abs(playerOne->getPosition().x - playerTwo->getPosition().x) * 1.0f;
	if (dist < 10)
		dist = 10;
	//camera->setPositionXY((playerTwo->getPosition().x + playerOne->getPosition().x) / 2.0f, abs(dist*0.1f) + 1.5f + ((playerTwo->getPosition().y + playerOne->getPosition().y) / 2.0f));
	if (dist > 40)
		dist = 40;
	//camera->setPositionZ(dist);

	//Make sure to do the reverse of the transform orders due to the change from row-major to column-major, it reverses all mathematic operations
	CameraTransform = Transform::Identity();
	//glm::rotate(CameraTransform, (-20.0f - abs(sqrtf(dist*0.01f)*10.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
	CameraTransform.RotateX(-20.0f - abs(sqrtf(dist*0.01f)*10.0f));
	//CameraTransform.Translate(vec3(0.0f, 7.5f, 11.0f));
	/*glm::translate(CameraTransform,
		glm::vec3((playerTwo->getPosition().x + playerOne->getPosition().x) / 2.0f, 
			abs(sqrtf(dist*0.01f)*20.0f) + 10.0f + ((playerTwo->getPosition().y + playerOne->getPosition().y) / 2.0f), 
			dist + 10));*/
	CameraTransform.Translate(glm::vec3((playerTwo->getPosition().x + playerOne->getPosition().x) / 2.0f, abs(sqrtf(dist*0.01f)*20.0f) + 10.0f + ((playerTwo->getPosition().y + playerOne->getPosition().y) / 2.0f), dist+10));
	//glm::rotate(CameraTransform, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	CameraTransform.RotateY(0.0f);
	//CameraTransform.RotateZ(180.0f);

	hudTransform = Transform::Identity();
	hudTransform.RotateY(0.0f);

	ShadowTransform = Transform::Identity();
	ShadowTransform.RotateX(45.0f);
	//.RotateY(180.0f);
	ShadowTransform.Translate(glm::vec3(0.0f, 10.0f, 10.0f));
	ShadowTransform.RotateY(180.0f);
	
	Transform bias = Transform(0.5f, 0.0f, 0.0f, 0.5f,
					 0.0f, 0.5f, 0.0f, 0.5f,
					 0.0f, 0.0f, 0.5f, 0.5f,
					 0.0f, 0.0f, 0.0f, 1.0f);

	ViewToShadowMap = Transform::Identity();
	ViewToShadowMap = bias * ShadowProjection * ShadowTransform.GetInverse() * CameraTransform;
	//ShadowTransform.Translate(vec3(0.0f, 0.0f, 0.0f));
}
/*
***Always remember to ask the three questions***
What are we rendering
Where are we rendering it to
How are we rendering it
*/
void Game::draw()
{
	/// Clear Buffers ///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.1f, 0.2f, 0.3f, 0);
	DeferredComposite.Clear();

	glClearColor(0.0f, 0.0f, 0.0f, 0);
	GBuffer.Clear();
	ShadowMap.Clear();
	HudMap.Clear();
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Generate The Shadow Map ///
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", ShadowProjection.data, true);

	ShadowMap.Bind();

	glBindVertexArray(Court.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Court.GetNumVertices());

	//glBindVertexArray(Background.VAO);
	//glDrawArrays(GL_TRIANGLES, 0, Background.GetNumVertices());

	//GBufferPass.SendUniformMat4("uModel",playerOne->transform.data, true);
	//playerOne->drawShadow(GBufferPass, 1);
	//GBufferPass.SendUniformMat4("uModel", playerTwo->transform.data, true);
	//playerTwo->drawShadow(GBufferPass, 1);

	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);

	GBufferPass.UnBind();
	//draw p1 shadow
	if (playerOne->action < 2 || (playerOne->action >= ACTION_JAB && playerOne->action <= ACTION_UP_ATTACK))
	{
		AniShader.Bind();
		AniShader.SendUniformMat4("uModel", Transform().data, true);
		AniShader.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
		AniShader.SendUniformMat4("uProj", ShadowProjection.data, true);

		AniShader.SendUniformMat4("uModel", playerOne->transform.data, true);
		playerOne->draw(AniShader, 0);

		AniShader.SendUniformMat4("uModel", Transform().data, true);
	}
	else {
		GBufferPass.Bind();
		GBufferPass.SendUniformMat4("uModel", Transform().data, true);
		GBufferPass.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
		GBufferPass.SendUniformMat4("uProj", ShadowProjection.data, true);
		GBufferPass.SendUniformMat4("uModel", playerOne->transform.data, true);
		playerOne->draw(GBufferPass, 0);
		GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	}
	//draw p2 shadow
	if (playerTwo->action < 2 || (playerTwo->action >= ACTION_JAB && playerTwo->action <= ACTION_UP_ATTACK))
	{
		AniShader.Bind();
		AniShader.SendUniformMat4("uModel", Transform().data, true);
		AniShader.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
		AniShader.SendUniformMat4("uProj", ShadowProjection.data, true);

		AniShader.SendUniformMat4("uModel", playerTwo->transform.data, true);
		playerTwo->draw(AniShader, 0);

		AniShader.SendUniformMat4("uModel", Transform().data, true);
	}
	else {
		GBufferPass.Bind();
		GBufferPass.SendUniformMat4("uModel", Transform().data, true);
		GBufferPass.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
		GBufferPass.SendUniformMat4("uProj", ShadowProjection.data, true);
		GBufferPass.SendUniformMat4("uModel", playerTwo->transform.data, true);
		playerTwo->draw(GBufferPass, 0);
		GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	}
	glBindVertexArray(0);

	ShadowMap.UnBind();
	AniShader.UnBind();

	/// Generate The Scene ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", Transform().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", CameraProjection.data, true);
	//GBufferPass.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);

	//MAKE SURE TO KNOW WHAT VIEWSPACE YOU ARE WORKING IN
	GBufferPass.SendUniform("uTex", 0);
	//GBufferPass.SendUniform("uShadowMap", 1);
	//StaticGeometry.SendUniform("LightPosition", CameraTransform.GetInverse() * vec4(4.0f, 0.0f, 0.0f, 1.0f));
	//Is .GetUp() the replacement for .GetRotationMat()?***
	/*GBufferPass.SendUniform("LightDirection", CameraTransform.GetInverse().GetUp() * ShadowTransform.GetForward());//vec4(ShadowTransform.GetForward(), 0.0f));
	GBufferPass.SendUniform("LightAmbient", vec3(0.333f, 0.333f, 0.333f)); //You can LERP through colours to make night to day cycles
	GBufferPass.SendUniform("LightDiffuse", vec3(0.8f, 0.8f, 0.8f));
	GBufferPass.SendUniform("LightSpecular", vec3(0.1f, 0.1f, 0.1f));
	GBufferPass.SendUniform("LightSpecularExponent", 200.0f);
	StaticGeometry.SendUniform("Attenuation_Constant", 1.0f);
	StaticGeometry.SendUniform("Attenuation_Linear", 0.1f);
	StaticGeometry.SendUniform("Attenuation_Quadratic", 0.01f);*/

	GBuffer.Bind();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE0);

	///playerOne->draw(GBufferPass);
	///playerTwo->draw(GBufferPass);
	
	/*SwordTexture.Bind();
	glBindVertexArray(Sword.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Sword.GetNumVertices());
	//Not sure if this is how to do this***
	//NormalSword.Bind();
	//glDrawArrays(GL_TRIANGLES, 1, Sword.GetNumVertices());

	int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
	glUniformMatrix4fv(modelLoc, 1, false, StoneTransform.data);

	StoneTexture.Bind();
	glBindVertexArray(Stone.VAO);

	// Adjust model matrix for next object's location
	glDrawArrays(GL_TRIANGLES, 0, Stone.GetNumVertices());
	glUniformMatrix4fv(modelLoc, 1, false, mat4().data);

	//Not sure if this is how to do this***
	//NormalStone.Bind();
	//glDrawArrays(GL_TRIANGLES, 1, Stone.GetNumVertices());

	//glUniformMatrix4fv(modelLoc, 1, false, 0);

	HouseTexture.Bind();
	glBindVertexArray(House.VAO);
	glDrawArrays(GL_TRIANGLES, 0, House.GetNumVertices());

	GroundTexture.Bind();
	glBindVertexArray(Ground.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Ground.GetNumVertices());
	//Using ground texture
	glBindVertexArray(Sphere.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Sphere.GetNumVertices());*/

	CourtTexture.Bind();
	glBindVertexArray(Court.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Court.GetNumVertices());

	BackgroundTexture.Bind();
	glBindVertexArray(Background.VAO);
	GBufferPass.SendUniformMat4("uModel", BGTransform.data, true);
	GBufferPass.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", CameraProjection.data, true);
	glDrawArrays(GL_TRIANGLES, 0, Background.GetNumVertices());
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	glBindVertexArray(0);

	//GBuffer.UnBind();

	CourtTexture.UnBind();

	for (unsigned int i = 0; i < Netbox.size(); i++) {
		int modelLoc = glGetUniformLocation(GBufferPass.getProgram(), "uModel");
		glUniformMatrix4fv(modelLoc, 1, false, Netbox[i]->getTransform().data);

		boxTexture.Bind();
		glBindVertexArray(boxMesh.VAO);

		// Adjust model matrix for next object's location
		glDrawArrays(GL_TRIANGLES, 0, boxMesh.GetNumVertices());
		glUniformMatrix4fv(modelLoc, 1, false, Transform().data);
	}
	boxTexture.UnBind();

	playerOne->drawBoxes(GBufferPass);
	playerTwo->drawBoxes(GBufferPass);
	//playerOne->draw(GBufferPass);
	//playerTwo->draw(GBufferPass);

	///Ani Shader///
	/*AniShader.Bind();
	static float aniTimer = 0.f;
	static int index = 0;
	aniTimer += updateTimer->getElapsedTimeSeconds();
	if (aniTimer > 1.0f)
	{
		aniTimer = 0.0f;
		index = (index + 1) % 2;
	}
	// Ask for the handles identfying the uniform variables in our shader.
	AniShader.SendUniformMat4("uModel", playerOne->transform.data, true);
	AniShader.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
	AniShader.SendUniformMat4("uProj", CameraProjection.data, true);
	AniShader.SendUniform("interp", aniTimer);
	AniShader.SendUniform("index", index);


	playerOne->draw(AniShader);
	playerTwo->draw(AniShader);

	AniShader.UnBind();*/

		AniShader.Bind();
		AniShader.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
		AniShader.SendUniformMat4("uProj", CameraProjection.data, true);
		playerOne->draw(AniShader, 1);

		AniShader.Bind();
		AniShader.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
		AniShader.SendUniformMat4("uProj", CameraProjection.data, true);
		playerTwo->draw(AniShader, 1);


	drawScore();

	AniShader.UnBind();
	GBuffer.UnBind();
	GBufferPass.UnBind();


	/// Detect Edges ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//Rednering it with the shader
	/*SobelPass.Bind();
	SobelPass.SendUniform("uNormalMap", 0);
	SobelPass.SendUniform("uDepthMap", 1);
	SobelPass.SendUniform("uPixelSize", vec2(1.0f / WINDOW_WIDTH, 1.0f / WINDOW_HEIGHT));

	//Where we are rendering
	EdgeMap.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetDepthHandle());
		DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	EdgeMap.UnBind();
	SobelPass.UnBind();*/

	/// Create Scene From GBuffer ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	DeferredLighting.Bind();
	DeferredLighting.SendUniformMat4("ViewToShadowMap", ViewToShadowMap.data, true);
	DeferredLighting.SendUniform("uScene", 0);
	DeferredLighting.SendUniform("uShadowMap", 1);
	DeferredLighting.SendUniform("uNormalMap", 2);
	DeferredLighting.SendUniform("uPositionMap", 3);
	//DeferredLighting.SendUniform("uEdgeMap", 4);
	//DeferredLighting.SendUniform("uStepTexture", 4);
	
	DeferredLighting.SendUniform("LightDirection", glm::vec3( CameraTransform.GetInverse().getRotationMat() * glm::normalize(ShadowTransform.GetForward())));
	DeferredLighting.SendUniform("LightAmbient", glm::vec3(0.8f, 0.8f, 0.8f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", glm::vec3(0.8f, 0.8f, 0.8f));
	DeferredLighting.SendUniform("LightSpecular", glm::vec3(0.8f, 0.8f, 0.8f));
	DeferredLighting.SendUniform("LightSpecularExponent", 500.0f);

	DeferredComposite.Bind();

	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ShadowMap.GetDepthHandle());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(1));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GBuffer.GetColorHandle(2));
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, EdgeMap.GetColorHandle(0));
	//glActiveTexture(GL_TEXTURE5);
	//StepTexture.Bind();
		DrawFullScreenQuad();
	//glBindTexture(GL_TEXTURE_2D, GL_NONE); //Could I do StepTexture.UnBInd()?
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, GL_NONE);
	//glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE); //Why was this not here in week 10 vid?

	DeferredComposite.UnBind();
	DeferredLighting.UnBind();

	drawHUD();
	drawTime();

	/// Compute High Pass ///
	glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));

	//Moving data to the back buffer, at the same time as our last post process
	BloomHighPass.Bind();
	BloomHighPass.SendUniform("uTex", 0);
	BloomHighPass.SendUniform("uThreshold", BLOOM_THRESHOLD);

	WorkBuffer1.Bind();

	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	WorkBuffer1.UnBind();

	BloomHighPass.UnBind();

	/// Compute Blur ///
	glViewport(0, 0, (GLsizei)(WINDOW_WIDTH / BLOOM_DOWNSCALE), (GLsizei)(WINDOW_HEIGHT / BLOOM_DOWNSCALE));
	for (int i = 0; i < BLOOM_BLUR_PASSES; i++)
	{
		//Horizontal Blur
		BlurHorizontal.Bind();
		BlurHorizontal.SendUniform("uTex", 0);
		BlurHorizontal.SendUniform("uPixelSize", 1.0f / WINDOW_WIDTH);

		WorkBuffer2.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer2.UnBind();

		BlurHorizontal.UnBind();

		//Vertical Blur
		BlurVertical.Bind();
		BlurVertical.SendUniform("uTex", 0);
		BlurVertical.SendUniform("uPixelSize", 1.0f / WINDOW_HEIGHT);

		WorkBuffer1.Bind();

		glBindTexture(GL_TEXTURE_2D, WorkBuffer2.GetColorHandle(0));
		DrawFullScreenQuad();
		glBindTexture(GL_TEXTURE_2D, GL_NONE);

		WorkBuffer1.UnBind();

		BlurVertical.UnBind();
	}

	//drawHUD();


	/// Composite To Back Buffer ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	BloomComposite.Bind();
	BloomComposite.SendUniform("uScene", 0);
	BloomComposite.SendUniform("uBloom", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DeferredComposite.GetColorHandle(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, WorkBuffer1.GetColorHandle(0));
		DrawFullScreenQuad();
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	BloomComposite.UnBind();


	glutSwapBuffers();
}

void Game::drawHUD()
{
	//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	
	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);//create ortho
	gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	//hudTransform.Translate(vec3(WINDOW_WIDTH * -0.5f, WINDOW_HEIGHT* -0.5f, 0));
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	//Draw Player 1 HUD
	///draw quad for p1 pic
	Transform hudLoc = Transform::Identity();
	hudLoc.Scale(100.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.Translate(glm::vec3(-450, -360, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P1Hud.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///draw quad for p1 bar
	hudLoc = Transform::Identity();
	hudLoc.Scale(glm::vec3(100.0f * (playerOne->getMeter() / 200.0f), 100.0f, 100));
	hudLoc.RotateY(90.0f);
	hudLoc.Translate(glm::vec3(-450 - 0.7*(200.0f - playerOne->getMeter()), -360, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P1Bar.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	//Draw Player 2 HUD
	///draw quad for p2 pic
	hudLoc = Transform::Identity();
	hudLoc.Scale(100.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.Translate(glm::vec3(450, -360 , 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P2Hud.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///draw quad for p2 bar
	hudLoc = Transform::Identity();
	hudLoc.Scale(glm::vec3(100.0f * (playerTwo->getMeter() / 200.0f), 100.0f, 100));
	hudLoc.RotateY(90.0f);
	hudLoc.Translate(glm::vec3(450 + 0.7*(200.0f - playerTwo->getMeter()), -360, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	P2Bar.Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	P2Bar.UnBind();


	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	//DeferredComposite.UnBind();
	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	

}

void Game::drawScore() {

	GBuffer.Bind();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	//glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", CameraProjection.data, true);
	///score
	Transform hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(-1, 7, -7));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	time[score1 % 10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///score
	hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(2, 7, -7.1f));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	time[10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());
	///score
	hudLoc = Transform::Identity();
	hudLoc.Scale(2.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(5, 7, -7));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);
	time[score2 % 10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());


	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	GBufferPass.UnBind();
	GBuffer.UnBind();
}

void Game::drawTime()
{
	//DeferredComposite.Bind();
	DeferredComposite.Bind();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
	glDisable(GL_DEPTH_TEST);  // disable depth-testing
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//new projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//save old state
	glLoadIdentity();//reset
	//gluOrtho2D(0.0, 1.0, 0.0, 1.0);//create ortho
	gluOrtho2D((float)WINDOW_WIDTH * -0.5f, (float)WINDOW_WIDTH * 0.5f, (float)WINDOW_HEIGHT * -0.5f, (float)WINDOW_HEIGHT * 0.5f);//create ortho
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//save old state
	glLoadIdentity();//reset

	//////////////////////////
	//now ready to draw 2d
	//////////////////////////
	GBufferPass.Bind();
	hudTransform = Transform::Identity();
	//hudTransform.Translate(vec3(WINDOW_WIDTH * -0.5f, WINDOW_HEIGHT* -0.5f, 0));
	GBufferPass.SendUniformMat4("uView", hudTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", hudProjection.data, true);

	int timer = 300 - (int)TotalGameTime;
	if (timer < 0) timer = 0;
	int min = (int)timer / 60;
	int secT = ((int)timer % 60) / 10;
	int secO = ((int)timer % 60) % 10;
	if (min > 10) min = 10;
	if (secT >= 6) min = 10;
	if (secO > 10) min = 10;

	//Draw Time
	///min
	Transform hudLoc;
	hudLoc.Scale(40.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(-50, 280, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[min]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///speerate
	hudLoc = Transform();
	hudLoc.Scale(40.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(0, 280, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[10]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec tens
	hudLoc = Transform();
	hudLoc.Scale(40.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(50, 280, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[secT]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	///sec ones
	hudLoc = Transform();
	hudLoc.Scale(40.0f);
	hudLoc.RotateY(90.0f);
	hudLoc.RotateX(-90.0f);
	hudLoc.Translate(glm::vec3(120 , 280, 0));
	GBufferPass.SendUniformMat4("uModel", hudLoc.data, true);

	time[secO]->Bind();
	glBindVertexArray(HudObj.VAO);
	glDrawArrays(GL_TRIANGLES, 0, HudObj.GetNumVertices());

	//unbind last used texture
	time[secO]->UnBind();


	GBufferPass.UnBind();

	//restore projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();//restore state
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();//restore state

	DeferredComposite.UnBind();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
}



void Game::keyboardDown(unsigned char key, int mouseX, int mouseY)
{
	//std::cout << key << " : " << (int)key << std::endl;
	switch (key)
	{
	//case 27: // the escape key
		//break;
	case 'w': //w
		//inputs[0] = true;
		break;
	case 'd': //d
		//inputs[1] = true;
		break;
	case 's': //s
		//inputs[2] = true;
		break;
	case 'a': //a
		//inputs[3] = true;
		break;
	case 'v': //n
		//inputs[4] = true;
		break;
	case 'b': //m
		//inputs[5] = true;
		break;
	case 'i': //up
		//inputs2[0] = true;
		break;
	case 'l': //right
		//inputs2[1] = true;
		break;
	case 'k': //down
		//inputs2[2] = true;
		break;
	case 'j': //left
		//inputs2[3] = true;
		break;
	case '.': //a
		//inputs2[4] = true;
		break;
	case '/': //b
		//inputs2[5] = true;
		break;
	case 'q': // the 'q' key
		//exit(1);
		//std::cout << playerOne->getPosition().x << " , " << playerOne->getPosition().y * 2;
		break;
	}
}

void Game::keyboardUp(unsigned char key, int mouseX, int mouseY)
{
	switch(key)
	{
	case 'R': //w
	case 'r': //w
		score1 = 0;
		score2 = 0;
		playerOne->respawn();
		playerTwo->respawn();
		playerOne->setPosition(glm::vec3(-5, 0, 0));
		playerTwo->setPosition(glm::vec3(5, 0, 0));
		//updateTimer = new Timer();
		TotalGameTime = 0.0f;
		break;
	case 'd': //d
		//inputs[1] = false;
		break;
	case 's': //s
		//inputs[2] = false;
		break;
	case 'a': //a
		//inputs[3] = false;
		break;
	case 'v': //n
		//inputs[4] = false;
		break;
	case 'b': //m
		// inputs[5] = false;
		break;
	case 'i': //up
		//inputs2[0] = false;
		break;
	case 'l': //right
		//inputs2[1] = false;
		break;
	case 'k': //down
		//inputs2[2] = false;
		break;
	case 'j': //left
		//inputs2[3] = false;
		break;
	case '.': //a
		//inputs2[4] = false;
		break;
	case '/': //b
		//inputs2[5] = false;
		break;
	case 'q': // the 'q' key
		//exit(1);
		break;
	}
}

void Game::mouseClicked(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN) 
	{
		switch(button)
		{
		case GLUT_LEFT_BUTTON:

			break;
		case GLUT_RIGHT_BUTTON:
		
		    
			break;
		case GLUT_MIDDLE_BUTTON:

			break;
		}
	}
	else
	{

	}
}

/*
 * mouseMoved(x,y)
 * - this occurs only when the mouse is pressed down
 *   and the mouse has moved.  you are given the x,y locations
 *   in window coordinates (from the top left corner) and thus 
 *   must be converted to screen coordinates using the screen to window pixels ratio
 *   and the y must be flipped to make the bottom left corner the origin.
 */
void Game::mouseMoved(int x, int y)
{
}


void Game::updateInputs()
{

	//Controler Ready.
	//Keep last frames inputs saved for comparing, to track ButtonPress, and ButtonRelease.
	//if was pressed las frame but not this frame, ButtonRelease() == true
	//if  not pressed last frame, but is pressed this frame, ButtonPress() == true
	Aold = Anew;
	Bold = Bnew;
	Yold = Ynew;
	Xold = Xnew;
	Sold = Snew;
	LBold = LBnew;
	RBold = RBnew;
	LTold = LTnew;
	RTold = RTnew;
	//Update inputs
	XBoxController.DownloadPackets(2);

	if (XBoxController.GetConnected(0)) {
		//Allow Release and Press
		if (XBoxController.GetButton(0, Input::A)) {
			Anew = true;
			pad = true;
		}
		else Anew = false;
		if (XBoxController.GetButton(0, Input::B)) {
			Bnew = true;
			pad = true;
		}
		else Bnew = false;
		if (XBoxController.GetButton(0, Input::Y)) {
			Ynew = true;
			pad = true;
		}
		else Ynew = false;
		if (XBoxController.GetButton(0, Input::X)) {
			Xnew = true;
			pad = true;
		}
		else Xnew = false;
		if (XBoxController.GetButton(0, Input::RB)) {
			RBnew = true;
			pad = true;
		}
		else RBnew = false;
		if (XBoxController.GetButton(0, Input::LB)) {
			LBnew = true;
			pad = true;
		}
		else LBnew = false;

		XBoxController.GetTriggers(0, lTrig, rTrig);
		if (rTrig > 0.5) {
			RTnew = true;
			pad = true;
		}
		else RTnew = false;
		if (lTrig > 0.5) {
			LTnew = true;
			pad = true;
		}
		else LTnew = false;


		Input::Stick lStick, rStick;
		XBoxController.GetSticks(0, lStick, rStick);
		if ((lStick.xAxis <= -0.45 || lStick.xAxis >= 0.45))
			pad = true;

		//pressed
		if (lStick.xAxis >= 0.9)//if key held
		{
			inputs[7] = true;
		}
		if (lStick.xAxis <= -0.9)//if key held
		{
			inputs[8] = true;
		}
		if (lStick.xAxis >= 0.45)//if key held
			inputs[1] = true;
		if (lStick.xAxis <= -0.45)//if key held
			inputs[3] = true;
		if (lStick.yAxis >= 0.7)//if key held
			inputs[0] = true;
		if (lStick.yAxis <= -0.7)//if key held
			inputs[2] = true;
		if (Xnew == true && Xold == false)
			inputs[4] = true;
		if (Ynew == true && Yold == false)
			inputs[5] = true;
		if (Anew == true && Aold == false)
			inputs[6] = true;
		if (RBnew == true && RBold == false)
			inputs[9] = true;

		//release
		if (lStick.xAxis < 0.9)//if key held
		{
			inputs[7] = false;
		}
		if (lStick.xAxis > -0.9)//if key held
		{
			inputs[8] = false;
		}
		if (lStick.xAxis < 0.45)//if key held
			inputs[1] = false;
		if (lStick.xAxis > -0.45)//if key held
			inputs[3] = false;
		if (lStick.yAxis < 0.7)//if key held
			inputs[0] = false;
		if (lStick.yAxis > -0.7)//if key held
			inputs[2] = false;
		if (Xnew == false && Xold == true)
			inputs[4] = false;
		if (Ynew == false && Yold == true)
			inputs[5] = false;
		if (Anew == false && Aold == true)
			inputs[6] = false;
		if (RBnew == false && RBold == true)
			inputs[9] = false;

	}
	else {

		inputs = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A
	}

	//PLAYER 2

	//Controler Ready.
	//Keep last frames inputs saved for comparing, to track ButtonPress, and ButtonRelease.
	//if was pressed las frame but not this frame, ButtonRelease() == true
	//if  not pressed last frame, but is pressed this frame, ButtonPress() == true
	Aold2 = Anew2;
	Bold2 = Bnew2;
	Yold2 = Ynew2;
	Xold2 = Xnew2;
	Sold2 = Snew2;
	LBold2 = LBnew2;
	RBold2 = RBnew2;
	LTold2 = LTnew2;
	RTold2 = RTnew2;

	if (XBoxController.GetConnected(1)) {
		//Allow Release and Press
		if (XBoxController.GetButton(1, Input::A)) {
			Anew2 = true;
			pad = true;
		}
		else Anew2 = false;
		if (XBoxController.GetButton(1, Input::B)) {
			Bnew2 = true;
			pad = true;
		}
		else Bnew2 = false;
		if (XBoxController.GetButton(1, Input::Y)) {
			Ynew2 = true;
			pad = true;
		}
		else Ynew2 = false;
		if (XBoxController.GetButton(1, Input::X)) {
			Xnew2 = true;
			pad = true;
		}
		else Xnew2 = false;
		if (XBoxController.GetButton(1, Input::RB)) {
			RBnew2 = true;
			pad = true;
		}
		else RBnew2 = false;
		if (XBoxController.GetButton(1, Input::LB)) {
			LBnew2 = true;
			pad = true;
		}
		else LBnew2 = false;

		XBoxController.GetTriggers(1, lTrig2, rTrig2);
		if (rTrig2 > 0.5f) {
			RTnew2 = true;
			pad = true;
		}
		else RTnew2 = false;
		if (lTrig2 > 0.5f) {
			LTnew2 = true;
			pad = true;
		}
		else LTnew2 = false;


		Input::Stick lStick2, rStick2;
		XBoxController.GetSticks(1, lStick2, rStick2);
		if ((lStick2.xAxis <= -0.45 || lStick2.xAxis >= 0.45))
			pad = true;


		//pressed
		if (lStick2.xAxis >= 0.9)//if key held
		{
			inputs2[7] = true;
		}
		if (lStick2.xAxis <= -0.9)//if key held
		{
			inputs2[8] = true;
		}
		if (lStick2.xAxis >= 0.45)//if key held
			inputs2[1] = true;
		if (lStick2.xAxis <= -0.45)//if key held
			inputs2[3] = true;
		if (lStick2.yAxis >= 0.7)//if key held
			inputs2[0] = true;
		if (lStick2.yAxis <= -0.7)//if key held
			inputs2[2] = true;
		if (Xnew2 == true && Xold2 == false)
			inputs2[4] = true;
		if (Ynew2 == true && Yold2 == false)
			inputs2[5] = true;
		if (Anew2 == true && Aold2 == false)
			inputs2[6] = true;
		if (RBnew2 == true && RBold2 == false)
			inputs2[9] = true;

		//released
		if (lStick2.xAxis < 0.9)//if key held
		{
			inputs2[7] = false;
		}
		if (lStick2.xAxis > -0.9)//if key held
		{
			inputs2[8] = false;
		}
		if (lStick2.xAxis < 0.45)//if key held
			inputs2[1] = false;
		if (lStick2.xAxis > -0.45)//if key held
			inputs2[3] = false;
		if (lStick2.yAxis < 0.7)//if key held
			inputs2[0] = false;
		if (lStick2.yAxis > -0.7)//if key held
			inputs2[2] = false;
		if (Xnew2 == false && Xold2 == true)
			inputs2[4] = false;
		if (Ynew2 == false && Yold2 == true)
			inputs2[5] = false;
		if (Anew2 == false && Aold2 == true)
			inputs2[6] = false;
		if (RBnew2 == false && RBold2 == true)
			inputs2[9] = false;
	}
	else {


		inputs2 = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A
	}
}

void Game::loadTime() {
	
	for (int i = 0; i <= 10; i++)
	{
		Texture * temp = new Texture();
		if (!temp->Load("./Assets/Textures/" + std::to_string(i) + ".png"))
		{
			std::cout << "Time Texture failed to load.\n";
			system("pause");
			exit(0);
		}
		time.push_back(temp);
	}
}