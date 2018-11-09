#include "Game.h"
#include "Utilities.h"

Game::Game()
	: GBuffer(3), DeferredComposite(1), ShadowMap(0), /*EdgeMap(1),*/ WorkBuffer1(1), WorkBuffer2(1)
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
	/*SobelPass.UnLoad();
	Sword.Unload();
	SwordTexture.Unload();
	Stone.Unload();
	StoneTexture.Unload();
	House.Unload();
	HouseTexture.Unload();
	Ground.Unload();
	GroundTexture.Unload();
	Sphere.Unload();
	StepTexture.Unload();*/
	Court.Unload();
	CourtTexture.Unload();
	//NormalSword.Unload();
	//NormalStone.Unload();
}

void Game::initializeGame()
{
	updateTimer = new Timer();

	//Only needs to be done once
	glEnable(GL_DEPTH_TEST);

	InitFullScreenQuad();

	inputs2 = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A

	playerOne = new Character("./Assets/Models/Knight.obj", "./Assets/Textures/player1.png");

	inputs = { false, false, false, false, false, false, false, false, false, false }; //up, left, down, right, X, Y, A

	playerTwo = new Character("./Assets/Models/Knight.obj", "./Assets/Textures/player2.png");


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

	if (!AniShader.Load("./Assets/Shaders/shaderAni.vert", "./Assets/Shaders/GBufferPass.frag"))
	{
		std::cout << "AS Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	/*if (!SobelPass.Load("./Assets/Shaders/PassThrough.vert", "./Assets/Shaders/Toon/Sobel.frag"))
	{
		std::cout << "SP Shaders failed to initialize.\n";
		system("pause");
		exit(0);
	}

	if (!Sword.LoadFromFile("./Assets/Models/Sword.obj"))
	{
		std::cout << "Sword Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!Stone.LoadFromFile("./Assets/Models/Stone.obj"))
	{
		std::cout << "Stone Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!House.LoadFromFile("./Assets/Models/House.obj"))
	{
		std::cout << "House Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!Ground.LoadFromFile("./Assets/Models/Ground.obj"))
	{
		std::cout << "Ground Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!Sphere.LoadFromFile("./Assets/Models/Sphere.obj"))
	{
		std::cout << "Sphere Model failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!SwordTexture.Load("./Assets/Textures/Sword.png"))
	{
		std::cout << "Sword Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!StoneTexture.Load("./Assets/Textures/Stone.png"))
	{
		std::cout << "Stone Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!HouseTexture.Load("./Assets/Textures/House.png"))
	{
		std::cout << "House Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	
	if (!GroundTexture.Load("./Assets/Textures/Ground1.png"))
	{
		std::cout << "Ground Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!StepTexture.Load("./Assets/Textures/StepTexture.png"))
	{
		std::cout << "Step Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	//My Cel-Shade is looking funky...
	StepTexture.SetNearestFilter();*/
	std::vector<std::string> court;
	court.push_back("./Assets/Models/KnightCourt.obj");
	Court.LoadFromFile(court);
	/*{
		std::cout << "Sword Model failed to load.\n";
		system("pause");
		exit(0);
	}*/

	if (!CourtTexture.Load("./Assets/Textures/GameCastleTexture.png"))
	{
		std::cout << "Court Texture failed to load.\n";
		system("pause");
		exit(0);
	}
	
	/*if (!NormalSword.Load("./Assets/Textures/SwordNormal.png"))
	{
		std::cout << "Sword Normal Texture failed to load.\n";
		system("pause");
		exit(0);
	}

	if (!NormalStone.Load("./Assets/Textures/StoneNormal.png"))
	{
		std::cout << "Stone Normal Texture failed to load.\n";
		system("pause");
		exit(0);
	}*/

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

	WorkBuffer1.InitColorTexture(0, WINDOW_WIDTH / BLOOM_DOWNSCALE, WINDOW_HEIGHT / BLOOM_DOWNSCALE, GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer1.CheckFBO())
	{
		std::cout << "WB1 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	WorkBuffer2.InitColorTexture(0, WINDOW_WIDTH / BLOOM_DOWNSCALE, WINDOW_HEIGHT / BLOOM_DOWNSCALE, GL_RGB8, GL_LINEAR, GL_CLAMP_TO_EDGE); //These parameters can be changed to whatever you want
	if (!WorkBuffer2.CheckFBO())
	{
		std::cout << "WB2 FBO failed to initialize.\n";
		system("pause");
		exit(0);
	}

	/*CameraTransform.Translate(vec3(0.0f, 7.5f, 20.0f));
	CameraTransform.RotateX(-15.0f);*/
	CameraProjection = mat4::PerspectiveProjection(60.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 10000.0f);
	//ShadowProjection.OrthographicProjection(35.0f, -35.0f, 35.0f, -35.0f, -10.0f, 100.0f);
	ShadowProjection = mat4::OrthographicProjection(-35.0f, 35.0f, 35.0f, -35.0f, -10.0f, 100.0f);
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
		playerOne->setPosition(playerOne->getPosition() + vec3(((diffx / abs(diffx))*0.01f), 0, 0));
		playerTwo->setPosition(playerTwo->getPosition() + vec3(((diffx / abs(diffx))*-0.01f), 0, 0));
	}

	//hitbox collisions placeholder
	int i = 0;
	bool done = false;
	while (done == false) {
		if (i >= playerTwo->getHitboxes().size()) {
			done = true;
		}
		else {
			float diffHx = playerOne->getPosition().x - playerTwo->getHitboxes()[i]->getPosition().x;//difference between characters x
			float diffHy = playerOne->getPosition().y - playerTwo->getHitboxes()[i]->getPosition().y;//difference between characters y
			if (abs(diffHx) < 0.3f + (playerTwo->getHitboxes()[i]->getSize() *0.1f) && (diffHy > -3.0f - (playerTwo->getHitboxes()[i]->getSize() *0.1f) && diffHy < 0.2f + +(playerTwo->getHitboxes()[i]->getSize() *0.1f))) {
				if (playerOne->blocking && (playerOne->facingRight != playerTwo->facingRight)) {//add only in front condition
					playerOne->blockSuccessful = true;
					playerTwo->getHitboxes()[i]->setDone();
					done = true;
				}
				else {
					playerOne->hit(playerTwo->getHitboxes()[i]);
					playerTwo->comboAdd();
					playerTwo->getHitboxes()[i]->setDone();
					done = true;
				}
			}
		}
		i++;
	}
	i = 0;
	done = false;
	while (done == false) {
		if (i >= playerOne->getHitboxes().size()) {
			done = true;
		}
		else {
			float diffHx = playerTwo->getPosition().x - playerOne->getHitboxes()[i]->getPosition().x;//difference between characters x
			float diffHy = playerTwo->getPosition().y - playerOne->getHitboxes()[i]->getPosition().y;//difference between characters y
			if (abs(diffHx) < 0.3f + (playerOne->getHitboxes()[i]->getSize() *0.1f) && (diffHy > -3.0f - (playerOne->getHitboxes()[i]->getSize() *0.1f) && diffHy < 0.2f + +(playerOne->getHitboxes()[i]->getSize() *0.1f))) {
				if (playerTwo->blocking && (playerOne->facingRight != playerTwo->facingRight)) {//add only in front condition
					playerTwo->blockSuccessful = true;
					playerOne->getHitboxes()[i]->setDone();
					done = true;
				}
				else {
					playerTwo->hit(playerOne->getHitboxes()[i]);
					playerOne->comboAdd();
					playerOne->getHitboxes()[i]->setDone();
					done = true;
				}
			}
		}
		i++;
	}
	updateInputs();
	playerOne->update(deltaTime, inputs2);
	playerTwo->update(deltaTime, inputs);

	//score
	if (abs(abs(playerOne->getPosition().x) - 14) < 1 && abs(playerOne->getPosition().y - 8) < 1) {
		playerOne->respawn();
		std::cout << std::endl << "Player 2 Scored" << std::endl;
	}
	if (abs(abs(playerTwo->getPosition().x) - 14) < 1 && abs(playerTwo->getPosition().y - 8) < 1) {
		playerTwo->respawn();
		std::cout << std::endl << "Player 1 Scored" << std::endl;
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
	CameraTransform = mat4::Identity;
	CameraTransform.RotateX(-20.0f);
	//CameraTransform.Translate(vec3(0.0f, 7.5f, 11.0f));
	CameraTransform.Translate(vec3((playerTwo->getPosition().x + playerOne->getPosition().x) / 2.0f, abs(sqrtf(dist*0.01f)*5.0f) + 7.0f + ((playerTwo->getPosition().y + playerOne->getPosition().y) / 2.0f), dist+5));
	CameraTransform.RotateY(0.0f);

	ShadowTransform = mat4::Identity;
	ShadowTransform.RotateX(-60.0f);
	ShadowTransform.Translate(vec3(0.0f, 8.0f, 25.0f));
	ShadowTransform.RotateY(-20.0f);
	
	mat4 bias = mat4(0.5f, 0.0f, 0.0f, 0.5f,
					 0.0f, 0.5f, 0.0f, 0.5f,
					 0.0f, 0.0f, 0.5f, 0.5f,
					 0.0f, 0.0f, 0.0f, 1.0f);

	ViewToShadowMap = mat4::Identity;
	ViewToShadowMap = bias * ShadowProjection * ShadowTransform.GetInverse() * CameraTransform;
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
	WorkBuffer1.Clear();
	WorkBuffer2.Clear();

	/// Generate The Shadow Map ///
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", mat4().data, true);
	//The reason of the inverse is because it is easier to do transformations
	GBufferPass.SendUniformMat4("uView", ShadowTransform.GetInverse().data, true);
	GBufferPass.SendUniformMat4("uProj", ShadowProjection.data, true);

	ShadowMap.Bind();

	/*glBindVertexArray(Sword.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Sword.GetNumVertices());

	glBindVertexArray(Stone.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Stone.GetNumVertices());

	glBindVertexArray(House.VAO);
	glDrawArrays(GL_TRIANGLES, 0, House.GetNumVertices());

	glBindVertexArray(Ground.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Ground.GetNumVertices());

	glBindVertexArray(Sphere.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Sphere.GetNumVertices());*/

	glBindVertexArray(Court.VAO);
	glDrawArrays(GL_TRIANGLES, 0, Court.GetNumVertices());
	GBufferPass.SendUniformMat4("uModel",playerOne->transform.data, true);
	playerOne->drawShadow(GBufferPass);
	GBufferPass.SendUniformMat4("uModel", playerTwo->transform.data, true);
	playerTwo->drawShadow(GBufferPass);

	GBufferPass.SendUniformMat4("uModel", mat4().data, true);
	GBufferPass.SendUniformMat4("uModel", mat4().data, true);

	glBindVertexArray(0);

	ShadowMap.UnBind();
	GBufferPass.UnBind();

	/// Generate The Scene ///
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	GBufferPass.Bind();
	GBufferPass.SendUniformMat4("uModel", mat4().data, true);
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
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, GL_NONE);

	glBindVertexArray(0);

	//GBuffer.UnBind();

	CourtTexture.UnBind();


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

	if (playerOne->action == ACTION_IDLE)
	{
		AniShader.Bind();
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
	}
	else
		playerOne->draw(GBufferPass);

	if (playerTwo->action == ACTION_IDLE)
	{
		AniShader.Bind();
		static float aniTimer = 0.f;
		static int index = 0;
		aniTimer += updateTimer->getElapsedTimeSeconds();
		if (aniTimer > 1.0f)
		{
			aniTimer = 0.0f;
			index = (index + 1) % 2;
		}
		// Ask for the handles identfying the uniform variables in our shader.
		AniShader.SendUniformMat4("uModel", playerTwo->transform.data, true);
		AniShader.SendUniformMat4("uView", CameraTransform.GetInverse().data, true);
		AniShader.SendUniformMat4("uProj", CameraProjection.data, true);
		AniShader.SendUniform("interp", aniTimer);
		AniShader.SendUniform("index", index);

		playerTwo->draw(AniShader);
	}
	else
		playerTwo->draw(GBufferPass);

	AniShader.UnBind();
	GBuffer.UnBind();
	GBufferPass.UnBind();

	/*SwordTexture.UnBind();
	StoneTexture.UnBind();
	HouseTexture.UnBind();
	GroundTexture.UnBind();*/
	//CourtTexture.UnBind();
	//StaticGeometry.UnBind(); //Why no longer unbind this?

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
	DeferredLighting.SendUniform("LightDirection", mat3(CameraTransform.GetInverse()) * ShadowTransform.GetForward());
	DeferredLighting.SendUniform("LightAmbient", vec3(0.8f, 0.8f, 0.8f)); //You can LERP through colours to make night to day cycles
	DeferredLighting.SendUniform("LightDiffuse", vec3(0.8f, 0.8f, 0.8f));
	DeferredLighting.SendUniform("LightSpecular", vec3(0.8f, 0.8f, 0.8f));
	DeferredLighting.SendUniform("LightSpecularExponent", 200.0f);

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

	/// Compute High Pass ///
	glViewport(0, 0, WINDOW_WIDTH / BLOOM_DOWNSCALE, WINDOW_HEIGHT / BLOOM_DOWNSCALE);

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
	glViewport(0, 0, WINDOW_WIDTH / BLOOM_DOWNSCALE, WINDOW_HEIGHT / BLOOM_DOWNSCALE);
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
	case 'w': //w
		//inputs[0] = false;
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
