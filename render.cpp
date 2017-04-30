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
        bool useFirstPixelToDetermineTransparencyColor = true)
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
    if (useFirstPixelToDetermineTransparencyColor && img->width > 0 && img->height > 0) {
        ta = *(data + 0), tb = *(data + 1), tc = *(data + 2);
    }
    for (i = 0; i < img->width * img->height * 3; i += 3) {
        a = *(data + 0);
        b = *(data + 1);
        c = *(data + 2);
        *(ptr + 0) = a;
        *(ptr + 1) = b;
        *(ptr + 2) = c;
        *(ptr + 3) = !(a == ta && a == tb && c == tc);
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

    glEnable(GL_MULTISAMPLE);

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

void initScenes()
{
    srand(time(NULL));
    initScenePlay();
    initSceneMenu();
    initScenePlayPause();
    initSceneCredits();
}

void initSceneMenu()
{
    Object* screenBg = new Object();
    screenBg->scene = GameSceneMenu;
    screenBg->objectType = ObjectTypeRectangle;
    screenBg->color = Color(0, 0, 0, 50);
    screenBg->pos.y = 0;
    screenBg->pos.x = 0;
    screenBg->dim.x = game.xres;
    screenBg->dim.y = game.yres;
    game.objects.push_back(screenBg);

    Object* menuBg = new Object();
    menuBg->scene = GameSceneMenu;
    menuBg->objectType = ObjectTypeRectangle;
    menuBg->color = Color(50, 50, 50, 150);
    menuBg->pos.y = 50;
    menuBg->pos.x = 200;
    menuBg->dim.x = 400;
    menuBg->dim.y = 500;
    game.objects.push_back(menuBg);

    const char* buttons[4] = {
        "New Game",
        "Restart",
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
        btnBg->color = Color(25, 25, 25, 255);
        btnBg->pos.y = 100 + i * 100;
        btnBg->pos.x = 250;
        btnBg->dim.x = 300;
        btnBg->dim.y = 90;
        game.objects.push_back(btnBg);

        Object* btnText = new Object();
        btnText->scene = GameSceneMenu;
        btnText->name = buttons[3 - i];
        btnText->objectType = ObjectTypeText;
        btnText->style = plain40;
        btnText->color = Color(210, 210, 210, 255);
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
    player->mass = 1;
    player->dim.x = -60; //
    player->dim.y = 40; //
    player->offset.x = 30;
    player->offset.y = 20;
    player->avgRadius = 0.25;
    mapTexture(player, "./images/bigfoot.ppm");
    game.objects.push_back(player);
    game.player = player;


    Object* ui = new Object();
    ui->scene = GameSceneHUD;
    ui->name = "ui";
    ui->objectType = ObjectTypeTexture;
    ui->pos.y = 0;
    ui->pos.x = 0;
    ui->mass = 0;
    ui->dim.x = 800;
    ui->dim.y = 600;
    mapTexture(ui, "./images/ui.ppm");
    game.objects.push_back(ui);



    Object* sun = new Object();
    sun->scene = GameSceneHUD;
    sun->name = "sun";
    sun->objectType = ObjectTypeTexture;
    sun->pos.y = 550;
    sun->pos.x = 750;
    sun->mass = 0;
    sun->dim.x = 100;
    sun->dim.y = 100;
    sun->avgRadius = 0.25;
    mapTexture(sun, "./images/sun.ppm");
    game.objects.push_back(sun);



    //Game Diagnostics Text
    Object* hRec = new Object();
    hRec->scene = GameSceneHUD;
    hRec->objectType = ObjectTypeRectangle;
    hRec->color = Color(255, 0, 0, 255);
    hRec->pos.y = game.yres - 42;
    hRec->pos.x = 385;
    hRec->dim.x = 140;
    hRec->dim.y = 20;
    hRec->name = "healthBar";
    game.objects.push_back(hRec);


    Object* healthText = new Object();
    healthText->scene = GameSceneHUD;
    healthText->objectType = ObjectTypeText;
    healthText->style = plain16;
    healthText->color = Color(0, 0, 0);
    healthText->intAttribute1 = 100;
    healthText->pos.y = game.yres - 45;
    healthText->pos.x = 440;
    healthText->name = "100";
    game.objects.push_back(healthText);

    Object* expRec = new Object();
    expRec->scene = GameSceneHUD;
    expRec->objectType = ObjectTypeRectangle;
    expRec->color = Color(5, 255, 5, 255);
    expRec->pos.y = game.yres - 70;
    expRec->pos.x = 385;
    expRec->dim.x = 0;
    expRec->dim.y = 20;
    expRec->name = "healthBar";
    game.objects.push_back(expRec);

    Object* expText = new Object();
    expText->scene = GameSceneHUD;
    expText->objectType = ObjectTypeText;
    expText->style = plain16;
    expText->color = Color(0, 0, 0);
    expText->intAttribute1 = 0;
    expText->pos.y = game.yres - 72;
    expText->pos.x = 440;
    expText->name = "0";
    game.objects.push_back(expText);

    Object* versionText = new Object();
    versionText->scene = GameSceneHUD;
    versionText->name = "Version 0.3";
    versionText->objectType = ObjectTypeText;
    versionText->style = plain16;
    versionText->color = Color(210, 210, 210, 255);
    versionText->pos.y = 10;
    versionText->pos.x = 670;
    game.objects.push_back(versionText);

    //generateFloorObjects(10);
    //Stationary Mines? Deals Large Damage 50 Health?
    for (int i = 0; i < 5; i++) {
        char name[12];
        int posx = rand() % 799;
        int posy = (rand() % (int) (((getOceanUpperBound(posx) - 20)
                -(getOceanFloorUpperBound(posx) + 20)))
                + (int) getOceanFloorUpperBound(posx));
        sprintf(name, "mine%d", i);
        Object* mine = new Object();
        mine->scene = GameScenePlay;
        mine->name = name;
        mine->objectType = ObjectTypeSphere;
        mine->color = Color(0, 0, 0);
        mine->pos.x = posx;
        mine->pos.y = posy;
        mine->avgRadius = 0.50;
        game.objects.push_back(mine);
    }

    for (int i = 0; i < 40; i++) {
        //char name[12];
        int posx = rand() % 799;
        int posy = (rand() % (int) (((getOceanUpperBound(posx) - 20)
                -(getOceanFloorUpperBound(posx) + 20)))
                + (int) getOceanFloorUpperBound(posx));
        //sprintf(name, "exp%d", i);
        Object* exp = new Object();
        exp->scene = GameScenePlay;
        exp->name = "exp";
        exp->objectType = ObjectTypeSphere;
        exp->color = Color(0, 255, 0);
        exp->pos.x = posx;
        exp->pos.y = posy;
        exp->avgRadius = 0.10;
        game.objects.push_back(exp);
    }


}

void initScenePlayPause()
{

}

void initSceneCredits()
{
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

void renderObjects(int scenesToRender) {
    //Rendered in order and let's hope it works
    for (int i = 0, l = game.objects.size(); i < l; ++i) {
        Object* obj = game.objects[i];
        if (scenesToRender & obj->scene) {
            switch (obj->objectType) {
            case ObjectTypeEnemy:
            case ObjectTypeFriendly:
                renderSphere(obj);
                break;
            case ObjectTypeSphere:
                renderSphere(obj);
                break;
            case ObjectTypeRectangle:
                renderRectangle(obj);
                break;
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

    if (game.scene & GameScenePlay) {
        glPushMatrix();
        game.cameraXMin = std::max(game.cameraXMin, game.player->pos.x - game.xres / 2);
        game.camera.x = std::max(game.cameraXMin, game.player->pos.x - game.xres / 2);
        glTranslatef(-game.camera.x, -game.camera.y, -game.camera.z);
        renderMap();
        renderObjects(GameScenePlay);
        glPopMatrix();
        if(game.scene & GameSceneHUD) {
            renderObjects(GameSceneHUD);
        }
    } else {
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
    obj->color.glChangeColor();
    glRectf(
            obj->pos.x,
            obj->pos.y,
            obj->pos.x + obj->dim.x,
            obj->pos.y + obj->dim.y
            );
}

void renderTexture(Object* obj)
{
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);
    glColor4ub(255, 255, 255, 255);
    glBindTexture(GL_TEXTURE_2D, obj->texId);
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
}

void renderText(Object* obj)
{
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
}

void mapTexture(Object* obj, const char* textureFile)
{
    //@TODO handle multiple file types? PPM, JPEG? ETC?
    obj->tex = ppm6GetImage(textureFile);
    glGenTextures(1, &obj->texId);
    int w = obj->tex->width;
    int h = obj->tex->height;
    glBindTexture(GL_TEXTURE_2D, obj->texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //TRANSPARENCY
    unsigned char *texAlphaData = buildAlphaData(
            obj->tex,
            obj->texTransUsingFirstPixel
            );
    glTexImage2D(
            GL_TEXTURE_2D, 0,
            GL_RGBA, w, h, 0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            texAlphaData
            );
    free(texAlphaData);
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






