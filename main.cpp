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
    uninitResources();
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

void handleEvents()
{
    XEvent event;
    while (XPending(game.display) > 0) {
        XNextEvent(game.display, &event);
        handleWindowResize(event);
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
    if(game.scene & GameSceneMenu && !(game.scene & GameSceneLogin)) {
        handleClickMenuItems(event);
    }
    if(game.scene & GameSceneLogin) {
        handleLoginInput(event);
    }
    if(game.scene & GameSceneUpgrades) {
        handleClickUpgradeItems(event);
    }
}


void handleClickMenuItems(const XEvent& event)
{
    if (event.type == ButtonPress) {
        int x = event.xbutton.x;
        int y = game.yres - event.xbutton.y;
        for (int i = 0, l = game.objects.size(); i < l; ++i) {
            Object* obj = game.objects[i];
            //If object is in menu scene and has a name
            //then it menu label
            if (obj->scene & (GameSceneMenu) && !obj->name.empty()) {
                if (y >= obj->  pos.y && y <= (obj->pos.y + obj->dim.y) &&
                        x >= obj->pos.x && x <= (obj->pos.x + obj->dim.x)) {
                    //then this button was pressed. Change state
                    if (obj->name == "Play") {
                        game.scene = GameScenePlay | GameSceneHUD;
                        game.start = clock();
						game.isGamePaused = false;
                    } else if (obj->name == "Help") {
                        game.scene = GameSceneHelp;
                    } else if (obj->name == "Credits") {
                        game.scene = GameSceneCredits;
                    } else if (obj->name == "Exit") {
                        game.done = true;
                    } else if (obj->name == "High Score") {
                        game.scene = GameSceneScore;
                    } else if (obj->name == "Upgrades") {
                        game.scene = GameSceneUpgrades;
                    } else if (obj->name == "Mute") {
						if(obj->objectType == ObjectTypeRectangle){
						if(obj->intAttribute1 == 0){
							obj->color = Color(51,204,255);
							obj->intAttribute1 = 1;
						}else{
							obj->color = Color(0,0,0,32);
							obj->intAttribute1 = 0;
					}
						muteAudio();
					}
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

void handleLoginInput(const XEvent& event)
{
    if (event.type == KeyPress) {
        char seq[32];
        KeySym keysym;
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        int len = XLookupString(const_cast<XKeyEvent*> (&event.xkey), seq, 25, &keysym, NULL);
        if (len > 0) {
            if (key == XK_Return && game.loginTxt->name != "<Enter>" && game.loginTxt->name.size() >= 4) {
                game.playerInfo.name = game.loginTxt->name;
                game.scene &= ~GameSceneLogin;
                updateHighScores(game.playerInfo.name, 0);
            } else if (key == XK_BackSpace && game.loginTxt->name != "<Enter>" && !game.loginTxt->name.empty()) {
                game.loginTxt->name = game.loginTxt->name.substr(0, game.loginTxt->name.size() - 1);
                if (game.loginTxt->name.empty()) {
                    game.loginTxt->name = "<Enter>";
                }
            } else if ( ('0' <= seq[0] && seq[0] <= '9') ||
                        ('A' <= seq[0] && seq[0] <= 'Z') ||
                        ('a' <= seq[0] && seq[0] <= 'z')) {
                if (game.loginTxt->name == "<Enter>") {
                    game.loginTxt->name = "";
                }
                if (game.loginTxt->name.size() < 20) {
                    game.loginTxt->name += seq[0];
                }
            }
        }
    }
}

void updateGameStats() {
    //Link the health bar to the health text int attribute
    if(game.healthTxt->intAttribute1 <= 0) {
	audioLoop();
	gameOver();
        game.scene = GameSceneMenu | GameSceneLost;
        game.lastScore = game.pointsTxt->intAttribute1;
        updateHighScores(game.playerInfo.name, game.lastScore);
        
        for(int i = game.preservedObjects, l = game.objects.size(); i < l; ++i) {
            delete game.objects[i];            
        }
        game.objects.erase(game.objects.begin() + game.preservedObjects, game.objects.end());
        game.healthTxt->intAttribute1 = 100;
        game.healthTxt->doubleAttribute1 = 100.0f;
        game.pointsTxt->intAttribute1 = 0;
        game.upgrade1->intAttribute1 = 0;
        game.upgrade2->intAttribute1 = 0;
        game.isGamePaused = true;
    }
    if(game.scene & GameScenePlay) {
        game.healthBar->dim.x = (
            game.healthTxt->intAttribute1 / 
            game.healthTxt->doubleAttribute1
        ) * 100.0f;
        std::stringstream ss; ss << game.healthTxt->intAttribute1;
        game.healthTxt->name = ss.str();
        ss.str(""); ss << game.pointsTxt->intAttribute1;
        game.pointsTxt->name = ss.str();
	ss.str("");
	if((game.timeTxt->doubleAttribute1 / 60) >= 1){
		ss << (int)(game.timeTxt->doubleAttribute1 / 60);
		ss << " m ";
	}
	ss << std::fmod((game.timeTxt->doubleAttribute1),60);
	ss << " s";
	game.timeTxt->name = ss.str();
	ss.str("");
	ss << game.highScoreTxt->intAttribute1;
	game.highScoreTxt->name = ss.str();
    }
    if(game.scene & GameSceneLost) {
        std::stringstream ss; ss << game.lastScore;
        game.pointsLast->name = ss.str();
    }
    if(game.scene & GameSceneUpgrades) {
        std::stringstream ss;
        ss.str("");
        ss << game.upgrade1->intAttribute1 << " (";
        ss << upgradeCurrentCost(game.upgrade1->intAttribute1) << ")";
        game.upgrade1->name = ss.str();

        ss.str("");
        ss << game.upgrade2->intAttribute1 <<" (";
        ss << upgradeCurrentCost(game.upgrade2->intAttribute1) << ")";
        game.upgrade2->name = ss.str();

        ss.str("");
        ss << availablePoints();
        game.spendingScoreTxt->name = ss.str();

    }
    if(game.scene & GameSceneScore) {
        std::stringstream ss;
        ss << game.playerInfo.highScore;
        game.highScoreTxt->name = ss.str();
        ss.str("");
        ss << game.playerInfo.totalScore;
        game.totalScoreTxt->name = ss.str();
    }
}
