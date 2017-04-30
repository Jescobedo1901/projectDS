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
    Object* enemy = new Object();
    enemy->scene = GameScenePlay;
    enemy->objectType = ObjectTypeEnemy;
    enemy->pos.x = game.player->pos.x + game.xres * 2;
    // rand for [min,max] = rand() % (max - min) + min
    int max = getOceanUpperBound(enemy->pos.x);
    int min = getOceanFloorUpperBound(enemy->pos.x);
    float rnd = (float) rand() / (float) RAND_MAX;
    enemy->pos.y = rand() % (max - min) + min;
    enemy->vel.x = -(rnd * 3 + 3);
    enemy->dim.x = 60;
    enemy->dim.y = 60;
    enemy->offset = Position(30, 30, 0);
    enemy->avgRadius = dimToAvgRadius(enemy->dim);
    enemy->mass = avgRadiusTOEstMass(enemy->avgRadius);
    enemy->intAttribute1 = 10; //hard coded for now
    enemy->texTransUsingFirstPixel = true;
    mapTexture(enemy, "./images/ojFish.jpg.ppm");
    game.objects.push_back(enemy);
}

void spawnFriendly()
{
    Object* enemy = new Object();
    enemy->scene = GameScenePlay;
    enemy->objectType = ObjectTypeFriendly;
    enemy->pos.x = game.player->pos.x + game.xres * 2;
    // rand for [min,max] = rand() % (max - min) + min
    int max = getOceanUpperBound(enemy->pos.x);
    int min = getOceanFloorUpperBound(enemy->pos.x);
    float rnd = (float) rand() / (float) RAND_MAX;
    enemy->pos.y = rand() % (max - min) + min;
    enemy->vel.x = -(rnd * 2 + 0.25);
    enemy->dim.x = 64;
    enemy->dim.y = 64;
    enemy->offset = Position(32, 32, 0);
    enemy->avgRadius = dimToAvgRadius(enemy->dim);
    enemy->mass = avgRadiusTOEstMass(enemy->avgRadius);
    enemy->intAttribute1 = 10; //hard coded for now
    enemy->texTransUsingFirstPixel = true;
    mapTexture(enemy, "./images/Cheeseburger.jpg");
    game.objects.push_back(enemy);
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
        break;
    case ObjectTypeFriendly:
        game.pointsTxt->intAttribute1 += other->intAttribute1;
        removeBag.insert(other);
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
            if(left == NULL || right == NULL) {
                printf("Hey we found an ull pointer at i = %d and j = %d]\n", i, j);
            }
                  
                    
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
            }
        }
    }
}
