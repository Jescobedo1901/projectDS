#include "game.h"

#ifdef USE_OPENAL_SOUND
#include </usr/include/AL/alut.h>
#endif //USE_OPENAL_SOUND

#ifndef DISABLE_AUDIO

#include <AL/al.h>
#include <AL/alut.h>
#include <AL/alc.h>

ALuint alBuffer[2]; //Number of Files
ALuint alSource[2];
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


////    alBuffer[0] = alutCreateBufferFromFile("./sound/menu.wav");
////    alBuffer[1] = alutCreateBufferFromFile("./sound/swim.wma");
////
////
////    alGenSources(2, alSource);
////    alSourcei(alSource[0], AL_BUFFER, alBuffer[0]);
////    alSourcei(alSource[1], AL_BUFFER, alBuffer[1]);
//
//    alSourcef(alSource[0], AL_GAIN, 1.0f);
//    alSourcef(alSource[0], AL_PITCH, 1.0f);
//    alSourcef(alSource[0], AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }

//    alSourcef(alSource[1], AL_GAIN, 0.6f);
//    alSourcef(alSource[1], AL_PITCH, 1.0f);
//    alSourcef(alSource[1], AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR) {
        printf("ERROR: settings\n");
    }
}

void uninitAudio()
{
    alDeleteSources(1, &alSource[0]);
    alDeleteSources(1, &alSource[1]);
    alDeleteBuffers(1, &alBuffer[0]);
    alDeleteBuffers(1, &alBuffer[1]);

    ALCcontext *Context = alcGetCurrentContext();
    ALCdevice *Device = alcGetContextsDevice(Context);
    alcMakeContextCurrent(NULL);
    alcDestroyContext(Context);
    alcCloseDevice(Device);

}

void handleAudio()
{
//    //printf("entering audio\n");
//    alGetSourcei(alSource[0], AL_SOURCE_STATE, &state); // menu
//
//    if (game.scene == 2 && state != AL_PLAYING) {
//        alGetSourcei(alSource[1], AL_SOURCE_STATE, &state); // game scene
//        if (state == AL_PLAYING) {
//            alSourceStop(alSource[1]);
//        }
//        alSourcePlay(alSource[0]);
//    }
//    if (game.scene != 2 && state == AL_PLAYING) {
//        alSourceStop(alSource[0]);
//    }
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

#endif
