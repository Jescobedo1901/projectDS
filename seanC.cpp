//Sean Coates
//Group 4
//DeepSea Survival Game

#include "game.h"

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
            mapTexture(neutral, "./images/rock1.ppm");
            break;
        case 1: //coral 1
            mapTexture(neutral, "./images/coral1.ppm");
            break;
        case 2: //coral 2
            mapTexture(neutral, "./images/coral2.ppm");
            break;
        case 3: //coral 3
            mapTexture(neutral, "./images/coral3.ppm");
            break;
        default: //rock
            mapTexture(neutral, "./images/rock1.ppm");
            break;
        }
        game.objects.push_back(neutral);
    }
}

