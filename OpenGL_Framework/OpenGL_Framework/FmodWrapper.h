//Zachary Allen
//FmodWrapper is from in class tutorials

#pragma once	
#pragma comment(lib, "FMOD/lib/fmod_vc.lib")

#include <Windows.h>
#include <iostream>

#include "FMOD/inc/fmod.hpp"
#include"FMOD/inc/fmod_errors.h"



void FmodErrorCheck(FMOD_RESULT result);
struct Listener
{
	FMOD_VECTOR pos = { 0.0f, 0.0f, -1.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	//listeners head, how its rotated
	FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
	FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
};
//Sound engine class
class SoundEngine
{
public:
	~SoundEngine();
	FMOD::System    *system;
	void update();
	Listener listener;
	bool Init();



private:
	unsigned int     version;
	void            *extradriverdata = 0;
	bool init = false;
	FMOD_RESULT      result;

};
//Sound Class
class Sound
{
public:
	~Sound();
	bool Load(const char* fileName);
	FMOD::Channel* Play(bool loop);
	static void SetPosition(FMOD::Channel* thisChannel, FMOD_VECTOR newPos, FMOD_VECTOR newVel = {0.0f, 0.0f, 0.0f});

	FMOD::Sound     *sound;
	FMOD::Sound		*gameSound;
	FMOD_VECTOR pos = { -10.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	static SoundEngine engine;

private:
	FMOD::Channel   *channel = 0; //channel is the sound that is playing, ref this if u want to move etc. 
	bool init = false;
	static FMOD_RESULT result;
};