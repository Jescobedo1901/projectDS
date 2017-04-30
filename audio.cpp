#include "game.h"

#ifndef DISABLE_AUDIO

void initAudio() 
{
    printf("AUDIO ENABLED\n");
}

void uninitAudio()
{
    
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