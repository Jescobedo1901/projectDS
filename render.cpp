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

//ALPHA DATA FUNCTION

unsigned char *buildAlphaData(
        Ppmimage *img,
        bool firstPixel)
{
    //add 4th component to RGB stream...
    int i;
    int a, b, c;
    unsigned char *newdata, *ptr;
    unsigned char *data = (unsigned char *) img->data;
    newdata = (unsigned char *) malloc(img->width * img->height * 4);
    ptr = newdata;
    //Let's use top right corner pixel color to distinct texture transparenc
    unsigned char ta = 0, tb = 0, tc = 0;
    if (firstPixel && img->width > 0 && img->height > 0) {
        ta = *(data + 0), tb = *(data + 1), tc = *(data + 2);
    }
    for (i = 0; i < img->width * img->height * 3; i += 3) {
        a = *(data + 0);
        b = *(data + 1);
        c = *(data + 2);
        *(ptr + 0) = a;
        *(ptr + 1) = b;
        *(ptr + 2) = c;
        *(ptr + 3) = !(a == ta && b == tb && c == tc);
        ptr += 4;
        data += 3;
    }
    return newdata;
}

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
void addRes(std::string name, std::string path, float optFps = 10.0) {
    if(path.find_first_of("*") == std::string::npos) {
        game.resourceMap[name] = new TextureResource(path);
    } else {
        game.resourceMap[name] = new FlipBook(path, optFps);
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
    addRes("images/enemy1", "./images/ojFish.jpg");
    addRes("images/enemy2", "./images/anglerFish.jpg");  
    addRes("images/friendly2", "./images/goldCoin*.png");
    addRes("images/cheeseburger", "./images/Cheeseburger.jpg");
    addRes("images/rock1", "./images/rock1.ppm");
    addRes("images/coral1", "./images/coral1.ppm");
    addRes("images/coral2", "./images/coral2.ppm");
    addRes("images/coral3", "./images/coral3.ppm");
    
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
}

void initSceneMenu()
{
    Object* screenBg = new Object();
    screenBg->scene = GameSceneMenu;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 128);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    Object* menuBg = new Object();
    menuBg->scene = GameSceneMenu;
    menuBg->objectType = ObjectTypeRectangle;
    menuBg->color = Color(75, 75, 75, 128);
    menuBg->pos.y = 50;
    menuBg->pos.x = 200;
    menuBg->dim.x = 400;
    menuBg->dim.y = 500;
    game.objects.push_back(menuBg);

    const char* buttons[4] = {
        "Play",
        "Help",
        "Credits",
        "Exit"
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
        btnBg->pos.x = 250;
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
        btnTextShadow->pos.x = 304;
        game.objects.push_back(btnTextShadow);

        Object* btnText = new Object();
        btnText->scene = GameSceneMenu;
        btnText->name = buttons[3 - i];
        btnText->objectType = ObjectTypeText;
        btnText->style = plain40;
        btnText->color = Color(210, 210, 210);
        btnText->pos.y = 120 + i * 100;
        btnText->pos.x = 300;
        game.objects.push_back(btnText);

    }

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

void initSceneUpgrades()
{
    //handlePlayerUpgrades();
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






