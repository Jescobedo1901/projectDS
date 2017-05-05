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
    int rndDim = (int)(rand() % 60) + 30;

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
        enemy2->dim.x = rndDim;
        enemy2->dim.y = rndDim;
        enemy2->offset = enemy2->dim/2.0f;
        enemy2->avgRadius = dimToAvgRadius(enemy2->dim);
        enemy2->mass = avgRadiusTOEstMass(enemy2->avgRadius);
        enemy2->intAttribute1 = 20; //hard coded for now
	mapResource(enemy2, "images/enemy2");
        enemy2->name = "enemy2";
        game.objects.push_back(enemy2);
    } else if (rndNum >= .2 && rndNum < .3) {
        Object* enemy3 = new Object();
        enemy3->scene = GameScenePlay;
        enemy3->objectType = ObjectTypeEnemy;
        enemy3->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(enemy3->pos.x);
        int min = getOceanFloorUpperBound(enemy3->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        enemy3->pos.y = min+25;
        enemy3->vel.x = -(rndPos * 3 + 3);
        enemy3->dim.x = 35;
        enemy3->dim.y = 69;
        enemy3->offset = enemy3->dim/2.0f;
        enemy3->avgRadius = enemy3->dim.y / 2.0 / PIXEL_TO_METER;
        enemy3->mass = avgRadiusTOEstMass(enemy3->avgRadius);
        enemy3->intAttribute1 = 40;
	mapResource(enemy3, "images/enemy3");
        enemy3->name = "enemy3";
        game.objects.push_back(enemy3);
  } else if(rndNum >= .3 && rndNum < .305) {
        Object* ship = new Object();
        ship->scene = GameScenePlay;
        ship->objectType = ObjectTypeEnemy;
        ship->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        //int max = getOceanUpperBound(ship->pos.x);
        //int min = getOceanFloorUpperBound(ship->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        ship->pos.y = getOceanUpperBound(game.player->pos.x);
        ship->vel.x = 0;
        ship->dim.x = 200;
        ship->dim.y = 200;
        ship->offset = ship->dim/2.0f;
        ship->avgRadius = dimToAvgRadius(ship->dim);
        ship->mass = avgRadiusTOEstMass(ship->avgRadius);
        ship->intAttribute1 = 10;
		ship->doubleAttribute1 = 1; //hard coded for now
        mapResource(ship, "images/ship");
        game.objects.push_back(ship);	
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
        enemy1->dim.x = rndDim;
        enemy1->dim.y = rndDim;
        enemy1->offset = enemy1->dim/2.0f;
        enemy1->avgRadius = dimToAvgRadius(enemy1->dim);
        enemy1->mass = avgRadiusTOEstMass(enemy1->avgRadius);
        enemy1->intAttribute1 = 10; //hard coded for now
        mapResource(enemy1, "images/enemy1");
        enemy1->name = "enemy1";
        game.objects.push_back(enemy1);
    }
}

void spawnFriendly()
{
    float rndPos = (float) rand() / (float) RAND_MAX;
    float rndNum = (float) rand() / (float) RAND_MAX;

    if (rndNum < .10) {
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
        friendly1->name = "friendly1";
        mapResource(friendly1, "images/friendly1");
        game.objects.push_back(friendly1);
    }
    else if(rndNum >= .10 && rndNum < .19) {
		printf("spawned Treasure\n");
        Object* treasure = new Object();
        treasure->scene = GameScenePlay;
        treasure->objectType = ObjectTypeFriendly;
        treasure->pos.x = game.player->pos.x + game.xres * 2;
        // rand for [min,max] = rand() % (max - min) + min
        int max = getOceanUpperBound(treasure->pos.x);
        int min = getOceanFloorUpperBound(treasure->pos.x);
        //float rnd = (float) rand() / (float) RAND_MAX;
        treasure->pos.y = getOceanFloorUpperBound(treasure->pos.x);
        treasure->vel.x = 0;
        treasure->dim.x = 100;
        treasure->dim.y = 100;
        treasure->offset = Position(32, 32, 0);
        treasure->avgRadius = dimToAvgRadius(treasure->dim);
        treasure->mass = avgRadiusTOEstMass(treasure->avgRadius);
        treasure->intAttribute1 = 10; //hard coded for now
        mapResource(treasure, "images/treasure");
        game.objects.push_back(treasure);		
	}else{
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
        friendly2->name = "friendly2";
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
        if ( other->name == "friendly1" ) {
            game.healthTxt->intAttribute1 = std::min(
                game.healthTxt->intAttribute1 + other->intAttribute1,
                (int)(game.healthTxt->doubleAttribute1));
        }
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
        if (enemy->name == "enemy2") {
            enemy->intAttribute1 += other->intAttribute1;
            removeBag.insert(other);
        }
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
				audioLoop();
            } else if (game.scene & GameSceneCredits) {
                game.scene = GameSceneMenu;
            } else if (game.scene & GameSceneHelp) {
                game.scene = GameSceneMenu;
            } else if (game.scene & GameSceneUpgrades) {
                game.scene = GameSceneMenu;
            } else if (game.scene & GameSceneScore) {
                game.scene = GameSceneMenu;
            }
        }
    }
}

void initSceneUpgrades()
{
    Object* screenBg = new Object();
    screenBg->scene = GameSceneUpgrades;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 220);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    for (int i = 0; i < 2; i++) {
        Object* upgradesTitle = new Object();
        upgradesTitle->scene = GameSceneUpgrades;
        upgradesTitle->objectType = ObjectTypeText;
            if (i == 0) {
                upgradesTitle->name = "UPGRADES! ! ! !";
                upgradesTitle->color = Color(25, 225, 25);
                upgradesTitle->style = plain40;
            } else {
                upgradesTitle->name = "Press ESC to Exit!";
                upgradesTitle->color = Color(255, 255, 255);
                upgradesTitle->style = plain17;
            }
        upgradesTitle->pos.y = 550 + (i * 75);
        upgradesTitle->pos.x = game.xres*.45;
        game.objects.push_back(upgradesTitle);
    }

    Object* bonusBg = new Object();
    bonusBg->scene = GameSceneUpgrades;
    bonusBg->objectType = ObjectTypeRectangle;
    bonusBg->color = Color(25, 225, 25, 50);
    bonusBg->pos.y = 275;
    bonusBg->pos.x = 25;
    bonusBg->dim.x = 700;
    bonusBg->dim.y = 250;
    game.objects.push_back(bonusBg);

    const char* buttonsUp[2] = {
        "+ Speed",
        "+ Health"
    };

    for (int i = 0; i < 2; ++i) {
        Object* upgradeRect = new Object();
        upgradeRect->scene = GameSceneUpgrades;
        upgradeRect->name = buttonsUp[i];
        upgradeRect->objectType = ObjectTypeRectangle;
        upgradeRect->color = Color(0, 0, 0, 128);
        upgradeRect->pos.y = 300 + i * 100;
        upgradeRect->pos.x = 50;
        upgradeRect->dim.x = 650;
        upgradeRect->dim.y = 90;
        game.objects.push_back(upgradeRect);

        Object* upgradeShadow = new Object();
        upgradeShadow->scene = GameSceneUpgrades;
        upgradeShadow->name = buttonsUp[i];
        upgradeShadow->objectType = ObjectTypeText;
        upgradeShadow->style = plain40;
        upgradeShadow->color = Color(0, 0, 0);
        upgradeShadow->pos.y = 324 + i * 100;
        upgradeShadow->pos.x = 79;
        game.objects.push_back(upgradeShadow);

        Object* upgradeTxt = new Object();
        upgradeTxt->scene = GameSceneUpgrades;
        upgradeTxt->name = buttonsUp[i];
        upgradeTxt->objectType = ObjectTypeText;
        upgradeTxt->style = plain40;

        upgradeTxt->pos.y = 320 + i * 100;
        upgradeTxt->pos.x = 75;
        game.objects.push_back(upgradeTxt);

        Object* upgradeType = new Object();
        upgradeType->scene = GameSceneUpgrades;
        upgradeType->objectType = ObjectTypeText;
        upgradeType->style = plain40;

        upgradeType->pos.y = 320 + i * 100;
        upgradeType->pos.x = 250;

        game.objects.push_back(upgradeType);

        if (i == 0) {
            upgradeTxt->color = Color(25, 25, 210);
            upgradeType->color = Color(25, 25, 210);
            upgradeType->name = "0";
            game.upgrade1 = upgradeType;
        } else {
            upgradeTxt->color = Color(210, 25, 25);
            upgradeType->color = Color(210, 25, 25);
            upgradeType->name = "0";
            game.upgrade2 = upgradeType;
        }
    }

    Object* pointTotalBg = new Object();
    pointTotalBg->scene = GameSceneUpgrades;
    pointTotalBg->objectType = ObjectTypeRectangle;
    pointTotalBg->color = Color(25, 225, 25, 50);
    pointTotalBg->pos.y = 125;
    pointTotalBg->pos.x = 25;
    pointTotalBg->dim.x = 700;
    pointTotalBg->dim.y = 125;
    game.objects.push_back(pointTotalBg);

    Object* pTotalRect = new Object();
    pTotalRect->scene = GameSceneUpgrades;
    pTotalRect->name = "Spending Points: ";
    pTotalRect->objectType = ObjectTypeRectangle;
    pTotalRect->color = Color(0, 0, 0, 128);
    pTotalRect->pos.y = 145;
    pTotalRect->pos.x = 50;
    pTotalRect->dim.x = 650;
    pTotalRect->dim.y = 90;
    game.objects.push_back(pTotalRect);

    Object* pTotalShadow = new Object();
    pTotalShadow->scene = GameSceneUpgrades;
    pTotalShadow->name = "Spending Points: ";
    pTotalShadow->objectType = ObjectTypeText;
    pTotalShadow->style = plain40;
    pTotalShadow->color = Color(0, 0, 0);
    pTotalShadow->pos.y = 169;
    pTotalShadow->pos.x = 79;
    game.objects.push_back(pTotalShadow);

    Object* pTotalTxt = new Object();
    pTotalTxt->scene = GameSceneUpgrades;
    pTotalTxt->name = "Spending Points ";
    pTotalTxt->objectType = ObjectTypeText;
    pTotalTxt->style = plain40;
    pTotalTxt->color = Color(255, 255, 255);
    pTotalTxt->pos.y = 165;
    pTotalTxt->pos.x = 75;
    game.objects.push_back(pTotalTxt);

    Object* spendingScoreTxt = new Object();
    spendingScoreTxt->scene = GameSceneUpgrades;
    spendingScoreTxt->objectType = ObjectTypeText;
    spendingScoreTxt->style = plain40;
    spendingScoreTxt->intAttribute1 = availablePoints();
    spendingScoreTxt->pos.y = 165;
    spendingScoreTxt->pos.x = 375;
    spendingScoreTxt->color = Color(255, 255, 255);
    spendingScoreTxt->name = "0";
    game.objects.push_back(spendingScoreTxt);
    game.spendingScoreTxt = spendingScoreTxt;
    

}

void handleClickUpgradeItems(const XEvent& event)
{
    //Total Points stored game.playerInfo.totalScore
    if (event.type == ButtonPress) {
    int x = event.xbutton.x;
    int y = game.yres - event.xbutton.y;
        for (int i = 0, l = game.objects.size(); i < l; ++i) {
            Object* obj = game.objects[i];
            //If object is in Upgrades Scene and has a name
            //then it's an Upgrade Option
            if (obj->scene & (GameSceneUpgrades) && !obj->name.empty()) {
                if (y >= obj->  pos.y && y <= (obj->pos.y + obj->dim.y) &&
                        x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                    //then this button was pressed!
                    if (obj->name == "+ Health") {
                        int avail = availablePoints();
                        int cost = upgradeCurrentCost(game.upgrade2->intAttribute1);
                        if(avail >= cost) {
                            game.upgrade2->intAttribute1++;
                            int increment = 10 * game.upgrade2->intAttribute1;
                            game.healthTxt->intAttribute1 += increment;
                            game.healthTxt->doubleAttribute1 += increment;
                            updateUsedPoints(cost);
                        } else {
                            playDmg();
                        }
                    } else if (obj->name == "+ Speed") {
                            int avail = availablePoints();
                            int cost = upgradeCurrentCost(game.upgrade1->intAttribute1);
                            if(avail >= cost) {
                            game.upgrade1->intAttribute1++;
                            game.thrustModifier = 100 * std::pow(1.05,
                            game.upgrade1->intAttribute1);
                            updateUsedPoints(cost);
                        } else {
                            playDmg();
                        }
                    }
                    playClick();
                    break;
                }
            }
        }
    }
}

int upgradeCurrentCost(int totalAllocated)
{
    return std::max(1 << totalAllocated, 1);
}

void updateUsedPoints(int pointLoss)
{
    game.usedScore += pointLoss;
}

int availablePoints()
{
    return (game.playerInfo.totalScore) - (game.usedScore) +
            (game.pointsTxt->intAttribute1);
}
