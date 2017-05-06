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
    uninitResources();
    uninitAudio();
    uninitGL();
    uninitX11();

    printf("Application terminating...\n");

    return 0;
}


//Initialize global game object
Game game;


//Game constructor default values

Game::Game() :
display(NULL),
root(),
win(),
vi(NULL),
cmap(),
swa(),
gwa(),
glc(NULL),
start(0),
done(false),
scene(GameSceneMenu | GameSceneLogin), //initialize new game to menu
isGamePaused(false),
lastButton(0),
mapBoundsIteration(0),
playerMovementDirectionMask(0),
xres(1600),
yres(900),
camera(),
cameraXMin(),
objects(),
player(NULL),
healthTxt(NULL),
healthBar(NULL),
pointsTxt(NULL),
pointsLast(NULL),
upgrade1(NULL),
upgrade2(NULL),
highScoreTxt(NULL),
totalScoreTxt(NULL),
thrustModifier(100),
resourceMap(),
lastScore(),
playerInfo("", 0, 0),
scoreObjects(),
preservedObjects()
{
}
