//Jacob Escobedo
//Group 4
//DeepSea Survival Game

#include "game.h"

void applyPlayerEnemyCollision(Object* player)
{
	Object* healthBar = game.objects[3];
	Object* healthNum = game.objects[4];
	Object* expBar = game.objects[5];
	Object* expNum = game.objects[6];
	//objects 3,4 Health 5,6 EXP
    for (int i = 0, l = game.objects.size(); i < l; i++) {
        Object* other = game.objects[i];
        if (other != player) {
            bool isColliding =
                    abs(player->pos - other->pos).magnitude()
                    - player->avgRadius
                    - other->avgRadius;
			bool circleCollision = false;//CircletoCircle Collision
			if(other->objectType == ObjectTypeSphere){
			if((pow(abs(player->pos.x - other->pos.x),2.0)+
				pow(abs(player->pos.y - other->pos.y),2.0))
			   	<= pow(((player->avgRadius + other->avgRadius)*
						PIXEL_TO_METER),2.0)){
				circleCollision = true;
				//printf("Collision\n");
			}
			}
            switch (other->objectType) {
            case ObjectTypeEnemy:
                if (isColliding) {
                    game.objects.erase(game.objects.begin() + i);
                    delete other;
                }
                break;
			case ObjectTypeSphere: //50 Damage Mine
					if(circleCollision) {
						char newname[10];
						if(strcmp(other->name.c_str(),"exp") != 0){
						game.objects.erase(game.objects.begin() + i);
						delete other;
						//int dmg = (1.4 * 50); //140px Bar/100
						if(healthNum->intAttribute1 >= 50){
						healthBar->dim.x -= (1.4 * 50);
						healthNum->intAttribute1 -= 50;
						}
						healthBar->pos.x = 385;
						sprintf(newname, "%d", healthNum->intAttribute1);
						healthNum->name = newname;
						}else{
						game.objects.erase(game.objects.begin() + i);
						delete other;
						//exp gives 10   100% = 1000exp ?
						expBar->dim.x += (1.4 * 1);
						expNum->intAttribute1 += 1;
						expBar->pos.x = 385;
						sprintf(newname, "%d", expNum->intAttribute1);
						expNum->name = newname;
						}
					}
					break;
            default:
                break;
            }
        }
    }
}

void handleESC(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Escape) {
            switch (game.scene) {
            case GameSceneMenu:
                if (game.isGamePaused) {
                    game.scene = GameScenePlay;
                    game.isGamePaused = false;
                } else {
                    game.done = true;
                }
                break;
            case GameScenePlay:
                game.scene = GameSceneMenu;
                game.isGamePaused = true;
                break;
                //                case GameScenePlayPause:
                //                    game.scene = GameScenePlay;
                //                    game.isGamePaused = false;
            case GameSceneCredits:
                game.scene = GameSceneMenu;
                break;
            default:
                break;
            }
        }
    }
}
