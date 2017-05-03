//Sean Coates
//Group 4
//DeepSea Survival Game

#include "game.h"

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
    //printf("numInWorld %d allObjects %d\n", numInWorld, allObjects);
    for (int n = numInWorld + 1; n <= 5; n++) {
        //printf("generating Object\n");

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
        //printf("type %d pos %d dimx %d dimy %d\n", type, pos, dimx, dimy);
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
        int y = 600 - event.xbutton.y;

        if (game.scene == 2 && game.lastButton == 0) {
            for (int i = 0, l = game.objects.size(); i < l; ++i) {
                Object* obj = game.objects[i];
                if (obj->scene == GameSceneMenu && !obj->name.empty()) {
                    //printf("x: %d y: %d objx: %f oby: %f button:%d\n", x, y, 
                    //obj->pos.x, obj->pos.y, game.lastButton);
                    if (y >= obj->pos.y && y <= (obj->pos.y + obj->dim.y) &&
                            x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                        playHover();
						if(obj->name != "Mute"){
                        obj->color = Color(51, 51, 51, 32);
                        game.lastButton = i;
						}else{
							game.lastButton = i;
						}
                    }
                }
            }
        } else {
            Object* obj = game.objects[game.lastButton];
            if (obj->scene == GameSceneMenu && !obj->name.empty()) {
                //printf("x: %d y: %d objx: %f oby: %f button:%d\n", x, y, 
                //obj->pos.x, obj->pos.y, game.lastButton);
                if ((y <= obj->pos.y || y >= (obj->pos.y + obj->dim.y)) ||
                        (x <= obj->pos.x || x >= (obj->pos.x + obj->dim.x))) {
					if(obj->name != "Mute"){
                    obj->color = Color(0, 0, 0, 32);
                    game.lastButton = 0;
					}else{
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
	int elapsedSec = (int)(current - game.start) * 1000.0/ CLOCKS_PER_SEC;
	elapsedSec = elapsedSec / 1000;
	Object* obj = game.objects[9];
	obj->doubleAttribute1 = elapsedSec;
}

