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
    initAudio();
    initResources();
    initScenes();
    gameLoop();
    //@TODO enable later, after changing all textures to alternate
    //formats to PPM
    //uninitResources();
    uninitAudio();
    uninitGL();
    uninitX11();

    printf("Application terminating...\n");

    return 0;
}

Game game;

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
lastButton(0),
mapBoundsIteration(0),
playerMovementDirectionMask(0),
xres(800),
yres(600),
camera(),
cameraXMin(),
objects(),
player(NULL),
healthTxt(NULL),
healthBar(NULL),
pointsTxt(NULL),
resourceMap()
{
}

void handleEvents()
{
    XEvent event;
    while (XPending(game.display) > 0) {
        XNextEvent(game.display, &event);
        handlePlayerMovement(event);
		handleMenuMouseMovement(event);
        handlePlayerClickExit(event);
        handleESC(event);
        handleMouseClicks(event);
		audioLoop();
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
                    if (obj->name == "Play") {
                        game.scene = GameScenePlay | GameSceneHUD;
                    } else if (obj->name == "Help") {
                        game.scene = GameSceneHelp;
                    } else if (obj->name == "Credits") {
                        game.scene = GameSceneCredits;
                    } else if (obj->name == "Exit") {
                        game.done = true;
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
    //Handled exit gracefully
    Atom wmDeleteMessage = XInternAtom(game.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(game.display, game.win, &wmDeleteMessage, 1);
    if (event.type == ClientMessage) {
        if (static_cast<unsigned long> (event.xclient.data.l[0]) == wmDeleteMessage) {
            game.done = true;
        }
    }
}


void updateGameStats() {
    //Link the health bar to the health text int attribute
    if(game.healthTxt->intAttribute1 <= 0) {
        game.scene = GameSceneMenu | GameSceneLost;
    }
    game.healthBar->dim.x = game.healthTxt->intAttribute1;
    std::stringstream ss; ss << game.healthTxt->intAttribute1;
    game.healthTxt->name = ss.str();
    ss.str(""); ss << game.pointsTxt->intAttribute1;
    game.pointsTxt->name = ss.str();
}