//Zachary Allen
//100650188
#include "FmodWrapper.h"
#include "FMOD/inc/fmod.hpp"

using namespace std;

void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		cout << FMOD_ErrorString(result) << endl;
		system("pause");
	}
}

SoundEngine::~SoundEngine()
{
	if (init)
	{
		result = system->close();
		FmodErrorCheck(result);
		result = system->release();
		FmodErrorCheck(result);
		init = false;
	}
}

void SoundEngine::update()
{
	//update the listener
	result = system->set3DListenerAttributes(0, &listener.pos, &listener.vel, &listener.forward, &listener.up);
	FmodErrorCheck(result);

	//sounds get played
	result = system->update(); //update function, dont put in game loop more than once
	FmodErrorCheck(result);
}

bool SoundEngine::Init()
{
	//Create a System object and initialize.

	if(!init)
	{
	result = FMOD::System_Create(&system);
	FmodErrorCheck(result);

	if (result != FMOD_OK) return false;

	result = system->getVersion(&version);
	FmodErrorCheck(result);

	result = system->init(100, FMOD_INIT_NORMAL, extradriverdata);
	FmodErrorCheck(result);

	//Set the distance units
	result = system->set3DSettings(1.0, 1.0, 1.0f);
	FmodErrorCheck(result);

	init = true;
	}

	return true;
}
FMOD_RESULT Sound::result;
SoundEngine Sound::engine;

Sound::~Sound()
{
	if (init)
	{
		result = sound->release();
		FmodErrorCheck(result);
	}
}

bool Sound::Load(const char * fileName)
{
	if (!init)
	{
		engine.Init();
		result = engine.system->createSound(fileName, FMOD_3D, 0, &sound);
		FmodErrorCheck(result);
		if (result != FMOD_OK) return false;

		result = sound->set3DMinMaxDistance(0.5f, 500.0f);
		FmodErrorCheck(result);
		init = true;
	}
	return true;


}

FMOD::Channel * Sound::Play(bool loop)
{
	
	

	if (init)
	{
		result = engine.system->playSound(sound, 0, true, &channel); //changing channel example
		FmodErrorCheck(result);
		result = channel->set3DAttributes(&pos, &vel);
		FmodErrorCheck(result);

		if (loop)
		{
			//if we want it to loop
			channel->setMode(FMOD_LOOP_NORMAL);
		}
		else
		{
			channel->setMode(FMOD_LOOP_OFF);
		}
	

		result = channel->setPaused(false);
		FmodErrorCheck(result);
	}

	return channel;
}

void Sound::SetPosition(FMOD::Channel * thisChannel, FMOD_VECTOR newPos, FMOD_VECTOR newVel)
{
	thisChannel->set3DAttributes(&newPos, &newVel);
}
