//Sean Coates
//Group 4
//DeepSea Survival Game

#include "game.h"
#define CWWidth (1<<2)
#define CWHeight (1<<3)

void playHover();

void generateFloorObjects(int numobj)
{
    srand(time(NULL));
    int numInWorld = 0;
    int allObjects = game.objects.size();
    char name[12];
    //printf("Object Function\n");
    for (int i = 0; i < allObjects; ++i) {
        Object* obj = game.objects[i];
        if (obj->objectType == ObjectTypeNeutral)
            numInWorld++;
    }
    for (int n = numInWorld + 1; n <= 5; n++) {
        int type = rand() % 4;
        int pos = (rand() % 799) + 1;
        int dimx = rand() % 151;
        int dimy = rand() % 101;
        if (dimx < 50) {
            dimx = 50;
        }
        if (dimy < 50) {
            dimy = 50;
        }
        Object* neutral = new Object();
        neutral->scene = GameScenePlay;
        sprintf(name, "neutral[%d]", n);
        neutral->name = name;
        neutral->objectType = ObjectTypeNeutral;
        neutral->pos.y = rand() % (int) (getOceanFloorUpperBound(pos) - 30);
        neutral->pos.x = pos;
        neutral->dim.x = dimx;
        neutral->dim.y = dimy;
        switch (type) {
        case 0: //rock
            mapResource(neutral, "images/rock1");
            break;
        case 1: //coral 1
            mapResource(neutral, "images/coral1");
            break;
        case 2: //coral 2
            mapResource(neutral, "images/coral2");
            break;
        case 3: //coral 3
            mapResource(neutral, "images/coral3");
            break;
        default: //rock
            mapResource(neutral, "/images/rock1");
            break;
        }
        game.objects.push_back(neutral);
    }
}

void handleMenuMouseMovement(const XEvent& event)
{
#ifndef DISABLE_AUDIO
    if (event.type == MotionNotify) {
        int x = event.xbutton.x;
        int y = game.yres - event.xbutton.y;

        if (    game.scene & GameSceneMenu && !(game.scene & GameSceneLogin) &&
                game.lastButton == 0) {
            for (int i = 0, l = game.objects.size(); i < l; ++i) {
                Object* obj = game.objects[i];
                if (obj->scene == GameSceneMenu && !obj->name.empty()) {
                    if (y >= obj->pos.y && y <= (obj->pos.y + obj->dim.y) &&
                            x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                        playHover();
                        if (obj->name != "Mute") {
                            obj->color = Color(51, 51, 51, 32);
                            game.lastButton = i;
                        } else {
                            game.lastButton = i;
                        }
                    }
                }
            }
        } else {
            Object* obj = game.objects[game.lastButton];
            if (obj->scene == GameSceneMenu && !obj->name.empty()) {
                if ((y <= obj->pos.y || y >= (obj->pos.y + obj->dim.y)) ||
                        (x <= obj->pos.x || x >= (obj->pos.x + obj->dim.x))) {
                    if (obj->name != "Mute") {
                        obj->color = Color(0, 0, 0, 32);
                        game.lastButton = 0;
                    } else {
                        game.lastButton = 0;
                    }
                }
            }
        }
    }

#endif
}

void elapsedTime()
{
    clock_t current = clock();
    int elapsedSec = (int) (current - game.start) * 1000.0 / CLOCKS_PER_SEC;
    elapsedSec = elapsedSec / 1000;
    Object* obj = game.objects[9];
    obj->doubleAttribute1 = elapsedSec;
}

void handleWindowResize(const XEvent& event)
{
    //XWindowAttributes attribs;
    if (event.type == ConfigureNotify) {
        if (    event.xconfigure.width != game.xres ||
                event.xconfigure.height != game.yres) {
            game.xres = event.xconfigure.width;
            game.yres = event.xconfigure.height;
            glViewport(0, 0, game.xres, game.yres);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glOrtho(0, game.xres, 0, game.yres, -1, 1);
        }
    }
}

void handleMouseClicks(const XEvent& event)
{
    if (game.scene & GameSceneMenu && !(game.scene & GameSceneLogin)) {
        handleClickMenuItems(event);
    }
    if (game.scene & GameSceneLogin) {
        handleLoginInput(event);
    }
    if (game.scene & GameSceneUpgrades) {
        handleClickUpgradeItems(event);
    }
}

void handleClickMenuItems(const XEvent& event)
{
    if (event.type == ButtonPress) {
        int x = event.xbutton.x;
        int y = game.yres - event.xbutton.y;
        for (int i = 0, l = game.objects.size(); i < l; ++i) {
            Object* obj = game.objects[i];
            //If object is in menu scene and has a name
            //then it menu label
            if (obj->scene & (GameSceneMenu) && !obj->name.empty()) {
                if (y >= obj-> pos.y && y <= (obj->pos.y + obj->dim.y) &&
                        x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                    //then this button was pressed. Change state
                    if (obj->name == "(P)lay") {
                        game.scene = GameScenePlay | GameSceneHUD;
                        game.start = clock();
                        game.isGamePaused = false;
                    } else if (obj->name == "(H)elp") {
                        game.scene = GameSceneHelp;
                    } else if (obj->name == "(C)redits") {
                        game.scene = GameSceneCredits;
                    } else if (obj->name == "(E)xit") {
                        game.done = true;
                    } else if (obj->name == "High (S)core") {
                        game.scene = GameSceneScore;
                    } else if (obj->name == "(U)pgrades") {
                        game.scene = GameSceneUpgrades;
                    } else if (obj->name == "(M)ute") {
                        if (obj->objectType == ObjectTypeRectangle) {
                            if (obj->intAttribute1 == 0) {
                                obj->color = Color(51, 204, 255);
                                obj->intAttribute1 = 1;
                            } else {
                                obj->color = Color(0, 0, 0, 32);
                                obj->intAttribute1 = 0;
                            }
                            muteAudio();
                        }
                    }
                    playClick();
                    break;
                }
            }
        }
    }
}

void handlePlayerClickExit(const XEvent& event)
{
    Atom wmDeleteMessage = XInternAtom(
            game.display, "WM_DELETE_WINDOW", False
    );
    XSetWMProtocols(game.display, game.win, &wmDeleteMessage, 1);
    if (event.type == ClientMessage) {
        if (    static_cast<unsigned long> (
                    event.xclient.data.l[0]) == wmDeleteMessage
                ) {
            game.done = true;
        }
    }
}




// AUDIO Section

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


    //button hover
    alBuffer[0] = alutCreateBufferFromFile("./sound/hover.wav");
    //button click
    alBuffer[1] = alutCreateBufferFromFile("./sound/click.wav");
    //point collection
    alBuffer[2] = alutCreateBufferFromFile("./sound/point.wav");
    //main loop
    alBuffer[3] = alutCreateBufferFromFile("./sound/woah.wav");
    //Dmg
    alBuffer[4] = alutCreateBufferFromFile("./sound/dmg.wav");
    //Game over
    alBuffer[5] = alutCreateBufferFromFile("./sound/gameover.wav");


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
    if (!muted) {
        playClick();
        //usleep(3000000);
        alSourcef(alSource[0], AL_GAIN, 0.0f);

        alSourcef(alSource[1], AL_GAIN, 0.0f);

        alSourcef(alSource[2], AL_GAIN, 0.0f);

        alSourcef(alSource[3], AL_GAIN, 0.0f);

        alSourcef(alSource[4], AL_GAIN, 0.0f);

        alSourcef(alSource[5], AL_GAIN, 0.0f);
        muted = true;
    } else {
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
    if (played == false) {
        alSourcePlay(alSource[5]);
        played = true;
    }
}

void audioLoop()
{
    //printf("audio\n");
    if (game.isGamePaused == true) {
        alGetSourcei(alSource[3], AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING)
            alSourceStop(alSource[3]);
    }
    if (game.scene == (GameSceneMenu | GameSceneLost)) {
        alGetSourcei(alSource[3], AL_SOURCE_STATE, &state);
        if (state == AL_PLAYING)
            alSourceStop(alSource[3]);
    }
    if (game.scene == (GameScenePlay | GameSceneHUD)) {
        if (game.isGamePaused == false) {
            //printf("audio\n");
            alGetSourcei(alSource[3], AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING)
                alSourcePlay(alSource[3]);
        }
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