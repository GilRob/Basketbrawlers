#pragma once
#include <windows.h>

//Must be included before the other includes since these include glew.h and its picky
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "ParticleEffect.h"
#include "Object.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "character.h"
#include "controller.h"
#pragma comment( lib, "Xinput9_1_0.lib" ) 

#include "Timer.h"

#define WINDOW_WIDTH			1280
#define WINDOW_HEIGHT			720
#define FULLSCREEN_WIDTH		1920
#define FULLSCREEN_HEIGHT		1080
#define FRAMES_PER_SECOND		60
#define BLOOM_THRESHOLD			0.33f
#define BLOOM_DOWNSCALE			3.0f
#define BLOOM_BLUR_PASSES		8	//How many times to repeat the blur process?
#define SHADOW_RESOLUTION		4096

class Game
{
public:
	Game();
	~Game();
	void sortObjects();
	Object* findObjects(std::string _name);

	void initializeGame();
	void update();
	void draw();
	void drawHUD();
	void loadTime();
	void drawTime();
	void drawScore();
	/* input callback functions */
	void keyboardDown(unsigned char key, int mouseX, int mouseY);
	void keyboardUp(unsigned char key, int mouseX, int mouseY);
	void mouseClicked(int button, int state, int x, int y);
	void mouseMoved(int x, int y);

	/* Data Members */
	Timer *updateTimer	= nullptr;
	float TotalGameTime = 0.0f;

	/// PROGRAMS ///
	ShaderProgram HudShader;
	ShaderProgram StaticGeometry;
	ShaderProgram GBufferPass;
	ShaderProgram BloomHighPass;
	//ShaderProgram SobelPass;
	ShaderProgram BlurHorizontal;
	ShaderProgram BlurVertical;
	ShaderProgram BloomComposite;
	ShaderProgram DeferredLighting;
	ShaderProgram AniShader;
	ShaderProgram PointLight;
	ShaderProgram ParticleProgram;

	Mesh boxMesh;
	Texture boxTexture;

	Character* playerOne;
	std::vector<bool> inputs2;
	Character* playerTwo;
	std::vector<bool> inputs;

	/// ENTITIES ///
	std::vector<Object*> gameObjects;
	Object* hitboxObj;

	Mesh HudObj;
	Texture P1Hud;
	Texture P1Bar;
	Texture P2Hud;
	Texture P2Bar;
	//Particle Effects
	ParticleEffect ConfettiEffectBlueRight;
	ParticleEffect ConfettiEffectBlueLeft;
	ParticleEffect ConfettiEffectRedRight;
	ParticleEffect ConfettiEffectRedLeft;
	std::vector<Texture*> time;
	std::vector<Hitbox*>Netbox;//2

	/// FRAMEBUFFERS ///
	FrameBuffer GBuffer; //Utility buffer to hold positions and normals
	FrameBuffer DeferredComposite; //Where the actual scene is loaded to
	FrameBuffer ShadowMap;
	//FrameBuffer EdgeMap;
	//Bloom work buffers
	FrameBuffer WorkBuffer1;
	FrameBuffer WorkBuffer2;
	FrameBuffer HudMap;

	Transform CameraTransform;
	Transform CameraProjection;
	Transform ShadowTransform;
	Transform ShadowProjection;
	Transform hudTransform;
	Transform hudProjection;

	Transform ViewToShadowMap;

	void updateInputs();
	//controller
	Input::XBoxInput XBoxController;

	bool Aold, Anew, Bold, Bnew, Yold, Ynew, Xold, Xnew, Snew, Sold;
	bool RBold, RBnew, LBold, LBnew, RTold, RTnew, LTold, LTnew;

	bool Aold2, Anew2, Bold2, Bnew2, Yold2, Ynew2, Xold2, Xnew2, Snew2, Sold2;
	bool RBold2, RBnew2, LBold2, LBnew2, RTold2, RTnew2, LTold2, LTnew2;
	bool pad;

	float lTrig, rTrig;
	float lTrig2, rTrig2;

	int score1;
	int score2;

	bool p1Score = false;
	bool p2Score = false;

private:

};
