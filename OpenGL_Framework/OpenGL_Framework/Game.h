#pragma once
#include <windows.h>

//Must be included before the other includes since these include glew.h and its picky
#include "ShaderProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "ParticleEffect.h"
#include "Object.h"
#include "PointLightObj.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "char_knight.h"
#include "char_ninja.h"
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
	void sortObjects(unsigned int scene);
	Object* findObjects(unsigned int scene, std::string _name);
	PointLightObj* findLight(std::string _name);

	void initializeGame();
	void update();
	void updateScene();
	void updateMenu();
	void updateSelect();
	void draw();
	void drawScene();
	void drawSelect();
	void drawHUD();
	void drawTime();
	void drawScore();
	void drawMenu();
	void loadTime();
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

	//Knight* playerOne;
	std::vector<bool> inputs2;
	//Ninja* playerTwo;
	std::vector<bool> inputs;

	Character* players[2];

	/// ENTITIES ///
	std::vector<Object*> gameObjects;
	std::vector<PointLightObj*> pointLights;
	Object* hitboxObj;
	std::vector<Object*> menuObjects;
	std::vector<Object*> selectObjects;

	unsigned int scene = 0; //0=menu, 1=selecting, 2=fighting
	bool gameDone = false;
	bool stun = true;

	//Menu
	unsigned int selectedButton = 0;
	float lastInputTime = 0.0f;

	//CSS
	unsigned int p1Char = 0;
	unsigned int p2Char = 0;
	bool p1Done = false;
	bool p2Done = false;

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
