// CMPS 3350 - Project: Deepsea Survival
// Authors:
//  Chris
//  Jake
//  Omar
//  Sean

#include "game.h"

int main(int argc, char** argv)
{
    printf("Starting application...\n");

    //Run main game loop until somebody makes a mistake
    //and stops playing the game
    initX11();
    initGL();
    initResources();
    initScenes();
    gameLoop();
    uninitGL();
    uninitX11();

    printf("Application terminating...\n");

    return 0;
}


Game game = Game();

Game::Game() :
display(NULL),
root(),
win(),
vi(NULL),
cmap(),
swa(),
gwa(),
glc(NULL),
done(false),
scene(GameSceneMenu), //initialize new game to menu
isGamePaused(false),
mapBoundsIteration(0),
playerMovementDirectionMask(0),
objects()
{
}

void handleEvents()
{
    XEvent event;
    while (XPending(game.display) > 0) {
        XNextEvent(game.display, &event);
        handlePlayerClickExit(event);
        handleMouseClicks(event);
    }
}

void handleMouseClicks(const XEvent& event)
{
    switch (game.scene) {
    case GameSceneMenu:
        handleClickMenuItems(event);
        break;
    default:
        break;
    }
}

void handleClickMenuItems(const XEvent& event)
{
    if (event.type == ButtonPress) {
        int x = event.xbutton.x;
        int y = 600 - event.xbutton.y;
        for (int i = 0, l = game.objects.size(); i < l; ++i) {
            Object* obj = game.objects[i];
            //If object is in menu scene and has a name
            //then it menu label
            if (obj->scene == GameSceneMenu && !obj->name.empty()) {
                if (y >= obj->pos.y && y <= (obj->pos.y + obj->dim.y) &&
                        x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                    //then this button was pressed. Change state    
                    if (obj->name == "New Game") {
                        game.scene = GameScenePlay;
                    } else if (obj->name == "Restart") {
                        //TBD
                    } else if (obj->name == "Credits") {
                        //TBD
                    } else if (obj->name == "Exit") {
                        game.done = true;
                    }
                    break;
                }
            }
        }
    }
}

void handlePlayerClickExit(const XEvent& event)
{
    //Handled exit gracefully
    Atom wmDeleteMessage = XInternAtom(game.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(game.display, game.win, &wmDeleteMessage, 1);
    if (event.type == ClientMessage) {
        if (static_cast<unsigned long> (event.xclient.data.l[0]) == wmDeleteMessage) {
            game.done = true;
        }
    }
}