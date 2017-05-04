#include "game.h"

#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif //USE_OPENAL_SOUND

#ifndef DISABLE_AUDIO

#include <AL/al.h>
#include <AL/alut.h>
#include <AL/alc.h>

ALuint alBuffer[6]; //Number of Files
ALuint alSource[6];
bool played = false;
bool muted = false;
ALint state;

void initAudio()
{
    printf("AUDIO ENABLED\n");
    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: alutInit()\n");
    }
    alGetError();

    float vec[6] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);


	alBuffer[0] = alutCreateBufferFromFile("./sound/hover.wav"); //button hover
	alBuffer[1] = alutCreateBufferFromFile("./sound/click.wav"); //button click
	alBuffer[2] = alutCreateBufferFromFile("./sound/point.wav"); //point collection
   	alBuffer[3] = alutCreateBufferFromFile("./sound/woah.wav");//main loop
	alBuffer[4] = alutCreateBufferFromFile("./sound/dmg.wav");//Dmg
	alBuffer[5] = alutCreateBufferFromFile("./sound/gameover.wav");//Game over

	
  	alGenSources(6, alSource);
    alSourcei(alSource[0], AL_BUFFER, alBuffer[0]);
  	alSourcei(alSource[1], AL_BUFFER, alBuffer[1]);
	alSourcei(alSource[2], AL_BUFFER, alBuffer[2]);
  	alSourcei(alSource[3], AL_BUFFER, alBuffer[3]);
	alSourcei(alSource[4], AL_BUFFER, alBuffer[4]);
	alSourcei(alSource[5], AL_BUFFER, alBuffer[5]);
//
  	alSourcef(alSource[0], AL_GAIN, 1.0f);
 	alSourcef(alSource[0], AL_PITCH, 1.0f);
  	alSourcef(alSource[0], AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }

 	alSourcef(alSource[1], AL_GAIN, 1.5f);
  	alSourcef(alSource[1], AL_PITCH, 1.2f);
  	alSourcef(alSource[1], AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
	
	alSourcef(alSource[2], AL_GAIN, 1.2f);
  	alSourcef(alSource[2], AL_PITCH, 1.0f);
  	alSourcef(alSource[2], AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
	alSourcef(alSource[3], AL_GAIN, 2.0f);
  	alSourcef(alSource[3], AL_PITCH, 1.0f);
  	alSourcef(alSource[3], AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
	alSourcef(alSource[4], AL_GAIN, 5.0f);
 	alSourcef(alSource[4], AL_PITCH, 0.9f);
  	alSourcef(alSource[4], AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
	
	alSourcef(alSource[5], AL_GAIN, 1.0f);
 	alSourcef(alSource[5], AL_PITCH, 1.0f);
  	alSourcef(alSource[5], AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
	
}

void muteAudio()
{
	if(!muted)
	{
		playClick();
		//usleep(3000000);
		alSourcef(alSource[0], AL_GAIN, 0.0f);

 		alSourcef(alSource[1], AL_GAIN, 0.0f);
	
		alSourcef(alSource[2], AL_GAIN, 0.0f);

		alSourcef(alSource[3], AL_GAIN, 0.0f);

		alSourcef(alSource[4], AL_GAIN, 0.0f);

		alSourcef(alSource[5], AL_GAIN, 0.0f);
		muted = true;
	}else{
		alSourcef(alSource[0], AL_GAIN, 1.0f);

 		alSourcef(alSource[1], AL_GAIN, 1.0f);
	
		alSourcef(alSource[2], AL_GAIN, 1.0f);

		alSourcef(alSource[3], AL_GAIN, 1.0f);

		alSourcef(alSource[4], AL_GAIN, 5.0f);

		alSourcef(alSource[5], AL_GAIN, 1.0f);
		muted = false;
	}

	
}


void uninitAudio()
{
    alDeleteSources(1, &alSource[0]);
    alDeleteSources(1, &alSource[1]);
	alDeleteSources(1, &alSource[2]);
    alDeleteSources(1, &alSource[3]);
	alDeleteSources(1, &alSource[4]);
	alDeleteSources(1, &alSource[5]);
	
    alDeleteBuffers(1, &alBuffer[0]);
    alDeleteBuffers(1, &alBuffer[1]);
    alDeleteBuffers(1, &alBuffer[2]);
    alDeleteBuffers(1, &alBuffer[3]);
	alDeleteBuffers(1, &alBuffer[4]);
	alDeleteBuffers(1, &alBuffer[5]);
	
    ALCcontext *Context = alcGetCurrentContext();
    ALCdevice *Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);

}



void playHover()
{

	alSourcePlay(alSource[0]);

}

void playClick()
{

	alSourcePlay(alSource[1]);

}

void playPoint()
{

	alSourcePlay(alSource[2]);

}

void playDmg()
{
	alSourcePlay(alSource[4]);
}

void gameOver()
{
	alGetSourcei(alSource[5], AL_SOURCE_STATE, &state);
	if(played == false){
		alSourcePlay(alSource[5]);
		played = true;
	}
}

void audioLoop()
{
	//printf("audio\n");

	if(game.scene == (GameSceneMenu | GameSceneLost)){
		alGetSourcei(alSource[3], AL_SOURCE_STATE, &state);
		if(state == AL_PLAYING)
			alSourceStop(alSource[3]);
	}
	if(game.scene == (GameScenePlay | GameSceneHUD)){
		//printf("audio\n");
		alGetSourcei(alSource[3], AL_SOURCE_STATE, &state);
		if(state != AL_PLAYING)
			alSourcePlay(alSource[3]);
	}
	
	//alSourcePlay(alSource[3]);

}

#else


//dummies
void initAudio()
{
    printf("AUDIO DISABLED\n");
}

void uninitAudio()
{
}

void playHover()
{
}

void playClick()
{
}

void playPoint()
{
}

void playDmg()
{
}

void gameOver()
{
}

void muteAudio()
{
}

void audioLoop()
{
}


#endif
