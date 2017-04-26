//Jacob Escobedo
//Group 4
//DeepSea Survival Game

#include "game.h"

void handlePlayerCollisions(Object* player) {
    bool isColliding = true;
    for(int i = 0, l = game.objects.size(); i < l; i++) {
        Object* other = game.objects[i];
        if(other != player) {
            switch(other->objectType) {
            case ObjectTypeEnemy:
                game.objects.erase(game.objects.begin() + i);
                delete other;
                break;
            default:
                break;
            }
        }
    }
	if(player->pos.y < getOceanFloorUpperBound(player->pos.y))
            	player->pos.y = getOceanFloorUpperBound(player->pos.x);
}


void handleESC(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Escape) {
            switch (game.scene) {
                case GameSceneMenu:
                    if(game.isGamePaused) {
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
