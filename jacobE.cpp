//Jacob Escobedo
//Group 4
//DeepSea Survival Game

#include "game.h"

bool isColliding(Object* left, Object* right, std::set<Object*>& removeBag);

void applySpawnRate(float stepDuration)
{
    /// random value between 0 and 1.0
    double rnd = (double) rand() / (double) RAND_MAX;

    //Setup equation
    double scale = 1.0;
    double pmax = 10000.0;
    double step = 100.0;
    int p = game.pointsTxt->intAttribute1; //bigger of the two

    //Calculate rate per second
    double ratePerSecond = scale * (((p % 100) / pmax) * ((p % 100) / pmax))
            + ((p * scale) / step) + 1;
    //                            ;    
    //
    double probabilityPerStep = ratePerSecond * stepDuration;

    if (rnd < probabilityPerStep) {
        spawnEnemy();
    }

    if(rnd < probabilityPerStep * 0.33) { 
        spawnFriendly();
    }

}

void spawnEnemy()
{
    //double rnd = (double) rand() / (double) RAND_MAX;
    float rndPos = (float) rand() / (float) RAND_MAX;
    float rndNum = (float) rand() / (float) RAND_MAX;
    
    if (rndNum < .2) {
        Object* enemy2 = new Object();
        enemy2->scene = GameScenePlay;
        enemy2->objectType = ObjectTypeEnemy;
        enemy2->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(enemy2->pos.x);
        int min = getOceanFloorUpperBound(enemy2->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        enemy2->pos.y = rand() % (max - min) + min;
        enemy2->vel.x = -(rndPos * 3 + 3);
        enemy2->dim.x = 60;
        enemy2->dim.y = 60;
        enemy2->offset = Position(30, 30, 0);
        enemy2->avgRadius = dimToAvgRadius(enemy2->dim);
        enemy2->mass = avgRadiusTOEstMass(enemy2->avgRadius);
        enemy2->intAttribute1 = 20; //hard coded for now
	mapResource(enemy2, "images/enemy2");
        game.objects.push_back(enemy2);
    } else {
        Object* enemy1 = new Object();
        enemy1->scene = GameScenePlay;
        enemy1->objectType = ObjectTypeEnemy;
        enemy1->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(enemy1->pos.x);
        int min = getOceanFloorUpperBound(enemy1->pos.x);
        float rnd = (float) rand() / (float) RAND_MAX;
        enemy1->pos.y = rand() % (max - min) + min;
        enemy1->vel.x = -(rndPos * 3 + 3);
        enemy1->dim.x = 60;
        enemy1->dim.y = 60;
        enemy1->offset = Position(30, 30, 0);
        enemy1->avgRadius = dimToAvgRadius(enemy1->dim);
        enemy1->mass = avgRadiusTOEstMass(enemy1->avgRadius);
        enemy1->intAttribute1 = 10; //hard coded for now
        mapResource(enemy1, "images/enemy1");
        game.objects.push_back(enemy1);
    }
}

void spawnFriendly()
{
    float rndPos = (float) rand() / (float) RAND_MAX;
    float rndNum = (float) rand() / (float) RAND_MAX;
    
    if (rndNum < .01) {
        Object* friendly1 = new Object();
        friendly1->scene = GameScenePlay;
        friendly1->objectType = ObjectTypeFriendly;
        friendly1->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(friendly1->pos.x);
        int min = getOceanFloorUpperBound(friendly1->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        friendly1->pos.y = rand() % (max - min) + min;
        friendly1->vel.x = -(rndPos * 1.1+ 0.25);
        friendly1->dim.x = 64;
        friendly1->dim.y = 64;
        friendly1->offset = Position(32, 32, 0);
        friendly1->avgRadius = dimToAvgRadius(friendly1->dim);
        friendly1->mass = avgRadiusTOEstMass(friendly1->avgRadius);
        friendly1->intAttribute1 = 10; //hard coded for now
        mapResource(friendly1, "images/friendly1");
        game.objects.push_back(friendly1);
    }
    else {
        Object* friendly2 = new Object();
        friendly2->scene = GameScenePlay;
        friendly2->objectType = ObjectTypeFriendly;
        friendly2->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(friendly2->pos.x);
        int min = getOceanFloorUpperBound(friendly2->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        friendly2->pos.y = rand() % (max - min) + min;
        friendly2->vel.x = -(rndPos * 2 + 0.25);
        friendly2->dim.x = 64;
        friendly2->dim.y = 64;
        friendly2->offset = Position(32, 32, 0);
        friendly2->avgRadius = dimToAvgRadius(friendly2->dim);
        friendly2->mass = avgRadiusTOEstMass(friendly2->avgRadius);
        friendly2->intAttribute1 = 1; //hard coded for now
        mapResource(friendly2, "images/friendly2");
        game.objects.push_back(friendly2);
    }
}

void applyPlayerCollision(Object* player, Object* other,
        std::set<Object*>& removeBag)
{
    switch (other->objectType) {
    case ObjectTypePlayer:
        break;
    case ObjectTypeEnemy:
        game.healthTxt->intAttribute1 -= other->intAttribute1;
        removeBag.insert(other);
		playDmg();
        break;
    case ObjectTypeFriendly:
        game.pointsTxt->intAttribute1 += other->intAttribute1;
        removeBag.insert(other);
        playPoint();
        break;
    case ObjectTypeNeutral:
        player->vel = (player->vel) / 2;
        removeBag.insert(other);
        break;
    default:
        break;
    }
}

void applyEnemyCollision(
        Object* enemy, Object* other,
        std::set<Object*>& removeBag)
{
    switch (other->objectType) {
    case ObjectTypePlayer:
        game.healthTxt->intAttribute1 -= enemy->intAttribute1;
        break;
    case ObjectTypeEnemy:
        enemy->intAttribute1 += other->intAttribute1;
        removeBag.insert(other);
        break;
    case ObjectTypeFriendly:
        //game.pointsTxt->intAttribute1 += other->intAttribute1;
        //removeBag.insert(other);
        break;
    case ObjectTypeNeutral:
        //player->vel = (player->vel) / 2;
        //removeBag.insert(other);
        break;
    default:
        break;
    }
}

void applyFriendlyCollision(
        Object* friendly, Object* other,
        std::set<Object*>& removeBag)
{

}

void applyNeutralCollision(
        Object* neutral, Object* other,
        std::set<Object*>& removeBag)
{

}

void checkObjectCollisions()
{
    //Holds objects that will be removed after loops
    std::set<Object*> removeBag;

    //std::vector<Object*> removeBag;
    for (int i = 0, l = game.objects.size(); i < l; i++) {
        for (int j = 0; j < l; j++) {
            Object* left = game.objects[i];
            Object* right = game.objects[j];
            
            //they are colliding...
            //Left will increment, handle from left perspective
            switch (left->objectType) {
            case ObjectTypePlayer:
                if (isColliding(left, right, removeBag))
                    applyPlayerCollision(left, right, removeBag);
                break;
            case ObjectTypeEnemy:
                if (isColliding(left, right, removeBag))
                    applyEnemyCollision(left, right, removeBag);
                break;
            case ObjectTypeFriendly:
                if (isColliding(left, right, removeBag))
                    applyFriendlyCollision(left, right, removeBag);
                break;
            case ObjectTypeNeutral:
                if (isColliding(left, right, removeBag))
                    applyNeutralCollision(left, right, removeBag);
                break;
            default:
                break;
            }
        }
    }
    /* http://stackoverflow.com/questions/2874441/
       deleting-elements-from-stl-set-while-iterating*/
   
    std::set<Object*>::iterator it = removeBag.begin();
    for (; it != removeBag.end(); it++) {
        game.objects.erase(std::remove(game.objects.begin(),
                game.objects.end(), *it));
        delete *it;
    }
}

bool isColliding(Object* left, Object* right, std::set<Object*>& removeBag)
{
    if(left->scene != GameScenePlay || right->scene != GameScenePlay) {
        return false;
    }
    if (removeBag.find(left) != removeBag.end() ||
            removeBag.find(right) != removeBag.end()) {
        return false;
    }
    if (left == right) {
        return false;
    }
    //Is left colliding with right
    float dist = (left->pos - right->pos).magnitude();
    float radiuses = left->avgRadius * PIXEL_TO_METER +
                    right->avgRadius * PIXEL_TO_METER;
    float effectiveDist = dist - radiuses;
    if ( effectiveDist <= 0.0 ) {
        return true;
    }
    return false;
}

void handleESC(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Escape) {
            if (game.scene & GameSceneMenu) {
                if (game.isGamePaused) {
                    game.scene = GameScenePlay | GameSceneHUD;
                    game.isGamePaused = false;
                } else {
                    game.done = true;
                }
            } else if (game.scene & GameScenePlay) {
                game.scene = GameSceneMenu;
                game.isGamePaused = true;
            } else if (game.scene & GameSceneCredits) {
                game.scene = GameSceneMenu;
            } else if (game.scene & GameSceneHelp) {
                game.scene = GameSceneMenu;
            }
        }
    }
}

void jHelpFunction()
{
    
}
