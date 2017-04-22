//Jacob Escobedo
//Group 4
//DeepSea Survival Game

#include "game.h"

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
                default:
                break;
            }
        }
    }
    
}