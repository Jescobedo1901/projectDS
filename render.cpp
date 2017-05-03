#include "game.h"

//Color class implementation - BEGIN

Color::Color() : r(0), g(0), b(0), a(0)
{
}

Color::Color(unsigned char _r, unsigned char _g, unsigned char _b)
: r(_r), g(_g), b(_b), a(255)
{
}

Color::Color(
        unsigned char _r,
        unsigned char _g,
        unsigned char _b,
        unsigned char _a)
: r(_r), g(_g), b(_b), a(_a)
{
}

void Color::glChangeColor()
{
    glColor4ub(r, g, b, a);
}

//To RGB no alpha

GLint Color::toRGBInt()
{
    return (r << 16) | (g << 8) | b;
}
//Color class implementation - END

void initX11()
{

    GLint glAttr[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

    if (!(game.display = XOpenDisplay(NULL))) {
        initFailure("X11: Failed opening X11 display");
    }

    game.root = DefaultRootWindow(game.display);

    if (!(game.vi = glXChooseVisual(game.display, 0, glAttr))) {
        initFailure("GL: Acquiring visual failed");
    }

    game.cmap = XCreateColormap(
            game.display,
            game.root,
            game.vi->visual,
            AllocNone
            );
    game.swa.colormap = game.cmap;
    game.swa.event_mask =
            ExposureMask | KeyPressMask | KeyReleaseMask |
            PointerMotionMask | MotionNotify | ButtonPress |
            ButtonRelease | StructureNotifyMask | SubstructureNotifyMask;
    game.win = XCreateWindow(
            game.display,
            game.root,
            0, 0,
            game.xres,
            game.yres,
            0,
            game.vi->depth,
            InputOutput,
            game.vi->visual,
            CWColormap | CWEventMask,
            &game.swa
            );

    XMapWindow(game.display, game.win);

    XStoreName(game.display, game.win, "Deepsea Survival");

    XkbSetDetectableAutoRepeat(game.display, 1, NULL);

    printf("X11 Initialized...\n");

}

void initGL()
{

    if (!(game.glc = glXCreateContext(
            game.display,
            game.vi,
            NULL,
            GL_TRUE))
            ) {
        initFailure("Failed creating new GLX context");
    }

    if (!glXMakeCurrent(
            game.display,
            game.win,
            game.glc)
            ) {
        initFailure("Failed making GLX current");
    }

    glViewport(
            0, 0,
            game.gwa.width,
            game.gwa.height
            );

    XGetWindowAttributes(
            game.display,
            game.win,
            &game.gwa
            );

    glOrtho(
            0,
            game.gwa.width,
            0,
            game.gwa.height,
            -1,
            1
            );

    glViewport(
            0, 0,
            game.gwa.width,
            game.gwa.height
            );
    //Initialize matrices
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //This sets 2D mode (no perspective)
    glOrtho(
            0,
            game.gwa.width,
            0,
            game.gwa.height,
            -1,
            1
            );

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    initialize_fonts();

}

void uninitX11()
{
    XDestroyWindow(game.display, game.win);
    XCloseDisplay(game.display);
}

void uninitGL()
{
    cleanup_fonts();
    glXMakeCurrent(game.display, None, NULL);
    glXDestroyContext(game.display, game.glc);
}

void uninitResources() {
    DIR *d;
    struct dirent *dir;
    d = opendir("./images");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                std::string ext(".ppm"), texFile(dir->d_name);
                //If textureFile does not end with .ppm, we must convert
                //it to .ppm first
                if(texFile.size() > ext.size() &&
                        texFile.compare(
                            texFile.size() - ext.size(), ext.size(), ext
                        ) == 0) {
                    std::string removingFile = std::string("./images/") +
                            dir->d_name;
                    remove(removingFile.c_str());
                }
            }
        }
    }
    for(ResourceMap::iterator it =
            game.resourceMap.begin(),
            end = game.resourceMap.end();
            it != end;
            ++it) {
        delete (*it).second;
    }
}

//Helper function to map and initialize resources, only used by initResources
void addRes(std::string name, std::string path, int tolerance = 0, float optFps = 10.0) {
    if(path.find_first_of("*") == std::string::npos) {
        game.resourceMap[name] = new TextureResource(path, tolerance);
    } else {
        game.resourceMap[name] = new FlipBook(path, tolerance, optFps);
    }
}

void initResources()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./images");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                std::string ext(".ppm"), texFile(dir->d_name);
                //If textureFile does not end with .ppm, we must convert it
                //to .ppm first
                if( texFile.size() > ext.size() &&
                        (texFile.compare(
                            texFile.size() - ext.size(), ext.size(), ext
                        ) != 0 &&
                        texFile.find_first_of(".") != 0)) {
                    std::string newFile = "./images/" + texFile + ext;
                    std::string command =
                            "convert \"./images/" + texFile + "\" \"" +
                            newFile + "\"";
                    system(command.c_str());
                }
            }
        }
        closedir(d);
    }

    addRes("images/player", "./images/bigfoot.ppm");
    addRes("images/enemy1", "./images/ojFish.jpg", 75);
    addRes("images/enemy2", "./images/anglerFish.jpg", 10);
    addRes("images/friendly2", "./images/goldCoin*.png", 50, 2);
    addRes("images/friendly1", "./images/Cheeseburger.jpg", 10);
    addRes("images/rock1", "./images/rock1.ppm");
    addRes("images/coral1", "./images/coral1.ppm");
    addRes("images/coral2", "./images/coral2.ppm");
    addRes("images/coral3", "./images/coral3.ppm");
    addRes("images/lost", "./images/lost.jpg", 150);
    addRes("images/logo", "./images/logo.jpg", 75);
}

void initScenes()
{
    srand(time(NULL));
    initScenePlay();
    initSceneMenu();
    initSceneHelp();
    initScenePlayPause();
    initSceneCredits();
    initSceneUpgrades();
    initSceneScore();
    game.preservedObjects = game.objects.size();
}

void initSceneMenu()
{
    Object* lostTxt = new Object();
    lostTxt->scene = GameSceneLost;
    lostTxt->objectType = ObjectTypeTexture;
    lostTxt->pos.x = 0;
    lostTxt->pos.y = -100;
    lostTxt->dim.x = 800;
    lostTxt->dim.y = 800;
    mapResource(lostTxt, "images/lost");
    game.objects.push_back(lostTxt);

    Object* screenBg = new Object();
    screenBg->scene = GameSceneMenu;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 128);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    Object* logo = new Object();
    logo->scene = GameSceneMenu;
    logo->objectType = ObjectTypeTexture;
    logo->pos.x = 100;
    logo->pos.y = game.yres-120;
    logo->dim.x = 600;
    logo->dim.y = 150;
    mapResource(logo, "images/logo");
	//game.logo = logo;
    game.objects.push_back(logo);

    Object* menuBg = new Object();
    menuBg->scene = GameSceneMenu;
    menuBg->objectType = ObjectTypeRectangle;
    menuBg->color = Color(75, 75, 75, 128);
    menuBg->pos.y = 75;
    menuBg->pos.x = 25;
    menuBg->dim.x = 350;
    menuBg->dim.y = 450;
    game.objects.push_back(menuBg);

    const char* buttons[5] = {
        "Play",
        "Help",
        "Credits",
        "Exit",
        "Mute"
    };

    for (int i = 0; i < 4; ++i) {
        Object* btnBg = new Object();
        btnBg->scene = GameSceneMenu;
        //Used to identify this box in the mouse click
        //handling logic
        btnBg->name = buttons[3 - i];
        btnBg->objectType = ObjectTypeRectangle;
        btnBg->color = Color(0, 0, 0, 32);
        btnBg->pos.y = 100 + i * 100;
        btnBg->pos.x = 50;
        btnBg->dim.x = 300;
        btnBg->dim.y = 90;
        game.objects.push_back(btnBg);

        Object* btnTextShadow = new Object();
        btnTextShadow->scene = GameSceneMenu;
        btnTextShadow->name = buttons[3 - i];
        btnTextShadow->objectType = ObjectTypeText;
        btnTextShadow->style = plain40;
        btnTextShadow->color = Color(0, 0, 0);
        btnTextShadow->pos.y = 124 + i * 100;
        btnTextShadow->pos.x = 104;
        game.objects.push_back(btnTextShadow);

        Object* btnText = new Object();
        btnText->scene = GameSceneMenu;
        btnText->name = buttons[3 - i];
        btnText->objectType = ObjectTypeText;
        btnText->style = plain40;
        btnText->color = Color(210, 210, 210);
        btnText->pos.y = 120 + i * 100;
        btnText->pos.x = 100;
        game.objects.push_back(btnText);

    }

    Object* soundBg = new Object();
    soundBg->scene = GameSceneMenu;
    soundBg->objectType = ObjectTypeRectangle;
    soundBg->color = Color(75, 75, 75, 128);
    soundBg->pos.y = 5;
    soundBg->pos.x = 200;
    soundBg->dim.x = 400;
    soundBg->dim.y = 40;
    game.objects.push_back(soundBg);

    Object* soundText = new Object();
    soundText->scene = GameSceneMenu;
    soundText->name = "Sound Option:";
    soundText->objectType = ObjectTypeText;
    soundText->style = plain16;
    soundText->color = Color(210, 210, 210);
    soundText->pos.y = 10;
    soundText->pos.x = 260;
    game.objects.push_back(soundText);

    Object* btnBg = new Object();
    btnBg->scene = GameSceneMenu;
    //Used to identify this box in the mouse click
    //handling logic
    btnBg->name = buttons[4];
    btnBg->objectType = ObjectTypeRectangle;
    btnBg->color = Color(0, 0, 0, 32);
    btnBg->intAttribute1 = 0;
    btnBg->pos.y = 8;
    btnBg->pos.x = 441;
    btnBg->dim.x = 65;
    btnBg->dim.y = 34;
    game.objects.push_back(btnBg);

    Object* muteText = new Object();
    muteText->scene = GameSceneMenu;
    muteText->name = buttons[4];
    muteText->objectType = ObjectTypeText;
    muteText->style = plain16;
    muteText->color = Color(210, 210, 210);
    muteText->pos.y = 10;
    muteText->pos.x = 455;
    game.objects.push_back(muteText);

    Object* menuBg2 = new Object();
    menuBg2->scene = GameSceneMenu;
    menuBg2->objectType = ObjectTypeRectangle;
    menuBg2->color = Color(75, 75, 75, 128);
    menuBg2->pos.y = 275;
    menuBg2->pos.x = game.xres-375;
    menuBg2->dim.x = 350;
    menuBg2->dim.y = 250;
    game.objects.push_back(menuBg2);

    const char* buttonsLeft[2] = {
        "Upgrades",
        "High Score"
    };

    for (int i = 0; i < 2; ++i) {
        Object* leftButts = new Object();
        leftButts->scene = GameSceneMenu;
        //Used to identify this box in the mouse click
        //handling logic
        leftButts->name = buttonsLeft[i];
        leftButts->objectType = ObjectTypeRectangle;
        leftButts->color = Color(0, 0, 0, 32);
        leftButts->pos.y = 300 + i * 100;
        leftButts->pos.x = game.xres-350;
        leftButts->dim.x = 300;
        leftButts->dim.y = 90;
        game.objects.push_back(leftButts);

        Object* leftButtsShadow = new Object();
        leftButtsShadow->scene = GameSceneMenu;
        leftButtsShadow->name = buttonsLeft[i];
        leftButtsShadow->objectType = ObjectTypeText;
        leftButtsShadow->style = plain40;
        leftButtsShadow->color = Color(0, 0, 0);
        leftButtsShadow->pos.y = 324 + i * 100;
        leftButtsShadow->pos.x = game.xres-296;
        game.objects.push_back(leftButtsShadow);

        Object* leftButtsText = new Object();
        leftButtsText->scene = GameSceneMenu;
        leftButtsText->name = buttonsLeft[i];
        leftButtsText->objectType = ObjectTypeText;
        leftButtsText->style = plain40;
        leftButtsText->color = Color(210, 210, 210);
        leftButtsText->pos.y = 320 + i * 100;
        leftButtsText->pos.x = game.xres-300;
        game.objects.push_back(leftButtsText);
    }

    Object* endScoreShadow = new Object();
    endScoreShadow->scene = GameSceneLost;
    endScoreShadow->name = "Score: ";
    endScoreShadow->objectType = ObjectTypeText;
    endScoreShadow->style = plain40;
    endScoreShadow->color = Color(255, 182, 193);
    endScoreShadow->pos.y = 101;
    endScoreShadow->pos.x = game.xres-373;
    game.objects.push_back(endScoreShadow);

    Object* endScore = new Object();
    endScore->scene = GameSceneLost;
    endScore->name = "Score: ";
    endScore->objectType = ObjectTypeText;
    endScore->style = plain40;
    endScore->color = Color(10, 180, 73);
    endScore->pos.y = 100;
    endScore->pos.x = game.xres-375;
    game.objects.push_back(endScore);

    Object* pointsLast = new Object();
    pointsLast->scene = GameSceneLost;
    pointsLast->objectType = ObjectTypeText;
    pointsLast->style = plain40;
    pointsLast->color = Color(255, 0, 50);
    pointsLast->intAttribute1 = game.lastScore;
    pointsLast->pos.y = 100;
    pointsLast->pos.x = game.xres-250;
    pointsLast->name = "0";
    game.objects.push_back(pointsLast);
    game.pointsLast = pointsLast;


    Object* loginBg = new Object();
    loginBg->scene = GameSceneLogin;
    loginBg->objectType = ObjectTypeRectangle;
    loginBg->color = Color(0, 0, 0, 128);
    loginBg->pos.y = 0;
    loginBg->pos.x = 0;
    loginBg->dim.x = game.xres;
    loginBg->dim.y = game.yres;
    game.objects.push_back(loginBg);

    Object* loginBox = new Object();
    loginBox->scene = GameSceneLogin;
    loginBox->objectType = ObjectTypeRectangle;
    loginBox->color = Color(0, 0, 0, 128);
    loginBox->pos.y = game.yres/3.0;
    loginBox->pos.x = game.xres/3.0;
    loginBox->dim.x = game.xres/3.0;
    loginBox->dim.y = game.yres/3.0;
    game.objects.push_back(loginBox);

    Object* useTxt = new Object();
    useTxt->scene = GameSceneLogin;
    useTxt->name = "Enter usename:";
    useTxt->objectType = ObjectTypeText;
    useTxt->style = plain17;
    useTxt->color = Color(255, 255, 255);
    useTxt->pos.y = game.yres/3.0+150;
    useTxt->pos.x = game.xres/3.0+25;
    game.objects.push_back(useTxt);

    Object* userInputTxt = new Object();
    userInputTxt->scene = GameSceneLogin;
    userInputTxt->name = "<Enter>";
    userInputTxt->objectType = ObjectTypeText;
    userInputTxt->style = plain17;
    userInputTxt->color = Color(255, 255, 255);
    userInputTxt->pos.y = game.yres/3.0+50;
    userInputTxt->pos.x = game.xres/3.0+25;
    game.objects.push_back(userInputTxt);
    game.loginTxt = userInputTxt;
}

void initScenePlay()
{
    Object* player = new Object();
    player->scene = GameScenePlay;
    player->name = "player";
    player->objectType = ObjectTypePlayer;
    player->pos.y = 150;
    player->pos.x = game.xres/2 + 5;
    player->dim.x = -60; //
    player->dim.y = 40; //
    player->offset.x = 30;
    player->offset.y = 20;
    player->avgRadius = dimToAvgRadius(player->dim);
    player->mass = avgRadiusTOEstMass(player->avgRadius);
    mapResource(player, "images/player");
    game.objects.push_back(player);
    game.player = player;

    Object* infoBg = new Object();
    infoBg->scene = GameSceneHUD;
    infoBg->objectType = ObjectTypeRectangle;
    infoBg->color = Color(0, 0, 0, 128);
    infoBg->pos.y = game.yres - 67;
    infoBg->pos.x = 3;
    infoBg->dim.x = 160;
    infoBg->dim.y = 65;
    game.objects.push_back(infoBg);

    Object* healthLabel = new Object();
    healthLabel->scene = GameSceneHUD;
    healthLabel->objectType = ObjectTypeText;
    healthLabel->style = plain16;
    healthLabel->color = Color(225, 0, 0);
    healthLabel->intAttribute1 = 100;
    healthLabel->pos.y = game.yres - 35;
    healthLabel->pos.x = 10;
    healthLabel->name = "HP: ";
    game.objects.push_back(healthLabel);
    game.healthTxt = healthLabel;

    Object* healthValue = new Object();
    healthValue->scene = GameSceneHUD;
    healthValue->objectType = ObjectTypeText;
    healthValue->style = plain16;
    healthValue->color = Color(255, 255, 255);
    healthValue->intAttribute1 = 100;
    healthValue->pos.y = game.yres - 35;
    healthValue->pos.x = 70;
    healthValue->name = "100";
    game.objects.push_back(healthValue);
    game.healthTxt = healthValue;

    Object* healthBgBack = new Object();
    healthBgBack->scene = GameSceneHUD;
    healthBgBack->objectType = ObjectTypeRectangle;
    healthBgBack->color = Color(255, 0, 0, 128);
    healthBgBack->pos.y = game.yres - 32;
    healthBgBack->pos.x = 55;
    healthBgBack->dim.x = 100;
    healthBgBack->dim.y = 17;
    game.objects.push_back(healthBgBack);
    game.healthBar = healthBgBack;

    Object* healthBg = new Object();
    healthBg->scene = GameSceneHUD;
    healthBg->objectType = ObjectTypeRectangle;
    healthBg->color = Color(0, 0, 0, 128);
    healthBg->pos.y = game.yres - 35;
    healthBg->pos.x = 55;
    healthBg->dim.x = 100;
    healthBg->dim.y = 3;
    game.objects.push_back(healthBg);

    Object* pointsLabel = new Object();
    pointsLabel->scene = GameSceneHUD;
    pointsLabel->objectType = ObjectTypeText;
    pointsLabel->style = plain16;
    pointsLabel->color = Color(0, 0, 225);
    pointsLabel->intAttribute1 = 0;
    pointsLabel->pos.y = game.yres - 62;
    pointsLabel->pos.x = 10;
    pointsLabel->name = "Points: ";
    game.objects.push_back(pointsLabel);

    Object* pointsValue = new Object();
    pointsValue->scene = GameSceneHUD;
    pointsValue->objectType = ObjectTypeText;
    pointsValue->style = plain16;
    pointsValue->color = Color(0, 0, 255);
    pointsValue->intAttribute1 = 0;
    pointsValue->pos.y = game.yres - 62;
    pointsValue->pos.x = 70;
    pointsValue->name = "0";
    game.objects.push_back(pointsValue);
    game.pointsTxt = pointsValue;

    Object* timeLabel = new Object();
    timeLabel->scene = GameSceneHUD;
    timeLabel->objectType = ObjectTypeText;
    timeLabel->style = plain16;
    timeLabel->color = Color(0, 0, 225);
    timeLabel->intAttribute1 = 0;
    timeLabel->pos.y = game.yres - 20;
    timeLabel->pos.x = game.xres - 150;
    timeLabel->name = "Time: ";
    game.objects.push_back(timeLabel);

    Object* timeValue = new Object();
    timeValue->scene = GameSceneHUD;
    timeValue->objectType = ObjectTypeText;
    timeValue->style = plain16;
    timeValue->color = Color(0, 0, 255);
    timeValue->doubleAttribute1 = 0.0;
    timeValue->pos.y = game.yres - 20;
    timeValue->pos.x = game.xres - 100;
    timeValue->name = "0.0";
    game.objects.push_back(timeValue);
    game.timeTxt = timeValue;



}

void initSceneHelp()
{
    Object* screenBg = new Object();
    screenBg->scene = GameSceneHelp;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 220);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    Object* title = new Object();
    title->scene = GameSceneHelp;
    title->name = "This is a sparring program..";
    title->objectType = ObjectTypeText;
    title->color = Color(25, 225, 25);
    title->style = plain40;
    title->pos.y = 525;
    title->pos.x = 300;
    game.objects.push_back(title);
}

void initScenePlayPause()
{

}

void initSceneCredits()
{
    Object* screenBg = new Object();
    screenBg->scene = GameSceneCredits;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 128);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    const char* names[5] = {
        "Press ESC key to exit!",
        "Sean C",
        "Marcel F",
        "Jacob E",
        "Omar O"
    };

    for (int i = 0; i < 5; ++i) {

        Object* nameText = new Object();
        nameText->scene = GameSceneCredits;
        nameText->name = names[i];
        nameText->objectType = ObjectTypeText;
        if (i == 0) {
            nameText->color = Color(255, 255, 255);
            nameText->style = plain17;
        } else {
            nameText->color = Color(255, 182, 193);
            nameText->style = plain40;
        }
        nameText->pos.y = 120 + i * 100;
        nameText->pos.x = 300;
        game.objects.push_back(nameText);
    }
}

void initSceneScore()
{
    int bottomOffset = game.yres / 6.0;
    int lineHeight= 40;
    int padLeft = game.xres / 4.0;

    Object* screenBg = new Object();
    screenBg->scene = GameSceneScore;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 128);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    Object* highScores = new Object();
    highScores->scene = GameSceneScore;
    highScores->name = "High Scores";
    highScores->objectType = ObjectTypeText;
    highScores->style = plain40;
    highScores->color = Color(0, 255, 255);
    highScores->pos.y = game.yres - 80;
    highScores->pos.x = padLeft;
    game.objects.push_back(highScores);

    for (unsigned int i = 0; i < 10; ++i) {

        Object* scoreRect = new Object();
        scoreRect->scene = GameSceneScore;
        scoreRect->objectType = ObjectTypeRectangle;
        scoreRect->color = Color(0, 0, 0, 32);
        scoreRect->pos.y = bottomOffset + i * lineHeight;
        scoreRect->pos.x = padLeft;
        scoreRect->dim.x = game.xres / 2.0;
        scoreRect->dim.y = lineHeight - 10;
        game.objects.push_back(scoreRect);

        Object* tsName = new Object();
        tsName->scene = GameSceneScore;
        tsName->objectType = ObjectTypeText;
        tsName->style = plain17;
        tsName->color = Color(210, 210, 210);
        tsName->pos.y = bottomOffset + i * lineHeight;
        tsName->pos.x = padLeft;
        game.objects.push_back(tsName);

        Object* tsMax = new Object();
        tsMax->scene = GameSceneScore;
        tsMax->objectType = ObjectTypeText;
        tsMax->style = plain17;
        tsMax->color = Color(210, 210, 210);
        tsMax->pos.y = bottomOffset + i * lineHeight;
        tsMax->pos.x = padLeft + 175;
        game.objects.push_back(tsMax);

        Object* tsTotal = new Object();
        tsTotal->scene = GameSceneScore;
        tsTotal->objectType = ObjectTypeText;
        tsTotal->style = plain17;
        tsTotal->color = Color(210, 210, 210);
        tsTotal->pos.y = bottomOffset + i * lineHeight;
        tsTotal->pos.x = padLeft + 300;
        game.objects.push_back(tsTotal);

        std::vector<Object*> vecs;
        vecs.push_back(tsName);
        vecs.push_back(tsMax);
        vecs.push_back(tsTotal);
        game.scoreObjects.push_back(vecs);
    }

    Object* yourScoreBg = new Object();
    yourScoreBg->scene = GameSceneScore;
    yourScoreBg->objectType = ObjectTypeRectangle;
    yourScoreBg->color = Color(0, 50, 0, 128);
    yourScoreBg->pos.y = 40;
    yourScoreBg->pos.x = padLeft;
    yourScoreBg->dim.x = game.xres / 2.0;
    yourScoreBg->dim.y = 40;
    game.objects.push_back(yourScoreBg);


    Object* yourScoreTxtLbl = new Object();
    yourScoreTxtLbl->scene = GameSceneScore;
    yourScoreTxtLbl->objectType = ObjectTypeText;
    yourScoreTxtLbl->name = "You:";
    yourScoreTxtLbl->style = plain17;
    yourScoreTxtLbl->color = Color(210, 210, 210);
    yourScoreTxtLbl->pos.y = 45;
    yourScoreTxtLbl->pos.x = padLeft;
    game.objects.push_back(yourScoreTxtLbl);

    Object* yourScoreTxtVal = new Object();
    yourScoreTxtVal->scene = GameSceneScore;
    yourScoreTxtVal->objectType = ObjectTypeText;
    yourScoreTxtVal->name = "5123";
    yourScoreTxtVal->style = plain17;
    yourScoreTxtVal->color = Color(210, 210, 210);
    yourScoreTxtVal->pos.y = 45;
    yourScoreTxtVal->pos.x = padLeft + 175;
    game.objects.push_back(yourScoreTxtVal);
    game.highScoreTxt = yourScoreTxtVal;

    Object* yourScoreTxtVal2 = new Object();
    yourScoreTxtVal2->scene = GameSceneScore;
    yourScoreTxtVal2->objectType = ObjectTypeText;
    yourScoreTxtVal2->name = "123";
    yourScoreTxtVal2->style = plain17;
    yourScoreTxtVal2->color = Color(210, 210, 210);
    yourScoreTxtVal2->pos.y = 45;
    yourScoreTxtVal2->pos.x = padLeft + 300;
    game.objects.push_back(yourScoreTxtVal2);
    game.totalScoreTxt = yourScoreTxtVal2;

}

void renderObjects(int scenesToRender) {
    //Rendered in order and let's hope it works
    for (int i = 0, l = game.objects.size(); i < l; ++i) {
        Object* obj = game.objects[i];
        if (scenesToRender & obj->scene) {
            switch (obj->objectType) {
            case ObjectTypeSphere:
                renderSphere(obj);
                break;
            case ObjectTypeRectangle:
                renderRectangle(obj);
                break;
            case ObjectTypeEnemy:
            case ObjectTypeFriendly:
            case ObjectTypePlayer:
            case ObjectTypeNeutral:
            case ObjectTypeTexture:
                renderTexture(obj);
                break;
            case ObjectTypeText:
                renderText(obj);
                break;
            default:
                break;
            }
        }
    }
}

void renderAll()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    if (game.scene & GameScenePlay || game.isGamePaused) {
        glPushMatrix();
        game.cameraXMin = std::max(
            game.cameraXMin, game.player->pos.x - game.xres / 2
        );
        game.camera.x = std::max(
            game.cameraXMin, game.player->pos.x - game.xres / 4
        );
        glTranslatef(-game.camera.x, -game.camera.y, -game.camera.z);
        renderMap();
        renderObjects(GameScenePlay);
        glPopMatrix();
        if(game.scene & GameSceneHUD) {
            renderObjects(GameSceneHUD);
        }
    }
    if(game.scene & ~(GameScenePlay | GameSceneHUD)) {
        renderMap();
        renderObjects(game.scene);
    }
	//audioLoop();
    glXSwapBuffers(game.display, game.win);
}

void renderMap()
{
    for (int x = game.camera.x; x < game.xres + game.camera.x; ++x) {
        glBegin(GL_LINES);
        glColor3ub(239, 245, 250);
        glVertex2d(x, getSkyUpperBound(x));
        glColor3ub(101, 188, 255);
        glVertex2f(x, getSkyLowerBound(x));

        glColor3ub(43, 175, 255);
        glVertex2d(x, getOceanUpperBound(x));
        glColor3ub(0, 75, 125);
        glVertex2f(x, getOceanFloorLowerBound(x));

        glColor3ub(221, 207, 18);
        glVertex2d(x, getOceanFloorUpperBound(x));
        glColor3ub(247, 223, 48);
        glVertex2f(x, getOceanFloorLowerBound(x));
        glEnd();
    }
}

void renderSphere(Object* obj)
{
    glBegin(GL_TRIANGLE_FAN);
    obj->color.glChangeColor();
    float rad = obj->avgRadius * PIXEL_TO_METER,
            x = obj->pos.x,
            y = obj->pos.y;
    for (int i = 0; i < 360; ++i) {
        float degInRad = i * M_PI / 180;
        glVertex2f(
                std::cos(degInRad) * rad + x,
                std::sin(degInRad) * rad + y
                );
    }
    glEnd();
}

void renderRectangle(Object* obj)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    obj->color.glChangeColor();
    glRectf(
            obj->pos.x,
            obj->pos.y,
            obj->pos.x + obj->dim.x,
            obj->pos.y + obj->dim.y
    );
    glDisable(GL_BLEND);
}

void renderTexture(Object* obj)
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);
    glBindTexture(GL_TEXTURE_2D, obj->resource->getResourceId());
    glBegin(GL_QUADS);
    float offsetX = obj->offset.x,
            offsetY = obj->offset.x,
            posX = obj->pos.x,
            posY = obj->pos.y,
            dimX = obj->dim.x,
            dimY = obj->dim.y;
    if (dimX < 0) {
        offsetX *= -1;
    }
    if (dimY < 0) {
        offsetY *= -1;
    }
    glTexCoord2f(0.0f, 1.0f);
    glVertex2i(posX - offsetX, posY - offsetY);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(posX - offsetX, posY - offsetY + dimY);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2i(posX - offsetX + dimX, posY - offsetY + dimY);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2i(posX - offsetX + dimX, posY - offsetY);
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
}

void renderText(Object* obj)
{
    glEnable(GL_TEXTURE_2D);
    Rect rect;
    rect.bot = obj->pos.y;
    rect.left = obj->pos.x;
    rect.center = 0;
    int cref = obj->color.toRGBInt();
    switch (obj->style) {
    case plain6:
        ggprint06(&rect, 0, cref, obj->name.c_str());
        break;
    case plain7:
        ggprint07(&rect, 0, cref, obj->name.c_str());
        break;
    case plain8:
        ggprint08(&rect, 0, cref, obj->name.c_str());
        break;
    case bold8:
        ggprint8b(&rect, 0, cref, obj->name.c_str());
        break;
    case plain10:
        ggprint10(&rect, 0, cref, obj->name.c_str());
        break;
    case plain12:
        ggprint12(&rect, 0, cref, obj->name.c_str());
        break;
    case plain13:
        ggprint13(&rect, 30, cref, obj->name.c_str());
        break;
    case plain16:
        ggprint16(&rect, 30, cref, obj->name.c_str());
        break;
    case plain17:
        ggprint17(&rect, 30, cref, obj->name.c_str());
        break;
    case plain40:
        ggprint40(&rect, 0, cref, obj->name.c_str());
        break;
    default:
        ggprint06(&rect, 0, cref, obj->name.c_str());
        break;
    }
    glDisable(GL_TEXTURE_2D);
}

float getSkyUpperBound(int x)
{
    return 600;
}

float getSkyLowerBound(int x)
{
    return 450;
}

float getOceanUpperBound(int x)
{
    return 500 + 10 * std::sin((x + game.mapBoundsIteration * 0.25) / 25.0);
}

float getOceanFloorUpperBound(int x)
{
    return 100 + 30 * std::sin(x / 100.0);
}

float getOceanFloorLowerBound(int x)
{
    return 0;
}






