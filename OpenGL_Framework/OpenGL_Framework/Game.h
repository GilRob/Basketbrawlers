#pragma once
#include <windows.h>

//Must be included before the other includes since these include glew.h and its picky
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "ParticleEffect.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "character.h"

#include "Timer.h"

#define WINDOW_WIDTH			1280
#define WINDOW_HEIGHT			720
#define FRAMES_PER_SECOND		60
#define BLOOM_THRESHOLD			0.33f
#define BLOOM_DOWNSCALE			2.0f
#define BLOOM_BLUR_PASSES		8	//How many times to repeat the blur process?
#define SHADOW_RESOLUTION		8192

class Game
{
public:
	Game();
	~Game();

	void initializeGame();
	void update();
	void draw();

	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	/* Data Members */
	Timer *updateTimer	= nullptr;
	float TotalGameTime = 0.0f;

	/// PROGRAMS ///
	ShaderProgram StaticGeometry;
	ShaderProgram GBufferPass;
	ShaderProgram BloomHighPass;
	//ShaderProgram SobelPass;
	ShaderProgram BlurHorizontal;
	ShaderProgram BlurVertical;
	ShaderProgram BloomComposite;
	ShaderProgram DeferredLighting;

	Character* playerOne;
	std::vector<bool> inputs;
	Character* playerTwo;
	std::vector<bool> inputs2;

	/// ENTITIES ///
	//Mesh Monkey;
	//Texture GrassTexture;
	/*Mesh Sword;
	Mesh Stone;
	Mesh House;
	Mesh Ground;
	Mesh Sphere;
	Texture SwordTexture;
	Texture StoneTexture;
	Texture HouseTexture;
	Texture GroundTexture;
	Texture StepTexture;*/
	//Texture NormalSword;
	//Texture NormalStone;
	Mesh Court;
	Texture CourtTexture;

	/// FRAMEBUFFERS ///
	FrameBuffer GBuffer; //Utility buffer to hold positions and normals
	FrameBuffer DeferredComposite; //Where the actual scene is loaded to
	FrameBuffer ShadowMap;
	//FrameBuffer EdgeMap;
	//Bloom work buffers
	FrameBuffer WorkBuffer1;
	FrameBuffer WorkBuffer2;


	mat4 StoneTransform;

	mat4 CameraTransform;
	mat4 CameraProjection;
	mat4 ShadowTransform;
	mat4 ShadowProjection;

	mat4 ViewToShadowMap;

private:

};
