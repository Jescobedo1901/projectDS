#include "game.h"

//Color class implementation - BEGIN
Color::Color() : r(0), g(0), b(0), a(0)
{            
}
Color::Color(unsigned char _r,unsigned char _g, unsigned char _b)
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
//To RGBA
GLint Color::toInt()
{
    return (r << 24) | (g << 16) | (b << 8) | a;
}
//To RGB no alpha
GLint Color::toRGBInt()
{
    return (r << 16) | (g << 8) | b;
}
//Color class implementation - END

void initX11() {

    GLint glAttr[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

    if (!(game.display = XOpenDisplay(NULL))) {
        initFailure("X11: Failed opening X11 display");
    }

    game.root = DefaultRootWindow(game.display);

    if (!(game.vi = glXChooseVisual(game.display, 0, glAttr))) {
        initFailure("GL: Acquiring visual failed");
    }

    game.cmap = XCreateColormap(game.display, game.root, game.vi->visual, AllocNone);
    game.swa.colormap = game.cmap;
    game.swa.event_mask = 
            ExposureMask | KeyPressMask | KeyReleaseMask |
            PointerMotionMask | MotionNotify | ButtonPress |
            ButtonRelease | StructureNotifyMask | SubstructureNotifyMask;
    game.win = XCreateWindow(
        game.display,
        game.root,
        0, 0,
        800,
        600,
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

void initGL() {
    
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
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

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

    glEnable(GL_MULTISAMPLE);

    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
        
    initialize_fonts();  
    
}

void uninitX11() {
    XDestroyWindow(game.display, game.win);
    XCloseDisplay(game.display); 
    
}

void uninitGL() {    
    cleanup_fonts();
    glXMakeCurrent(game.display, None, NULL);
    glXDestroyContext(game.display, game.glc);
}

void initScenes() {
    initSceneMenu();
    initScenePlay();
    initScenePlayPause();
    initSceneCredits();
}

void initSceneMenu() {
    Object* wall = new Object();
    wall->scene = GameSceneMenu;
    wall->objectType = ObjectTypePlayer;
    wall->color = Color(255, 255, 255);
    wall->avgRadius = 100;
    wall->pos.y = 0;
    wall->pos.x = 0;
    wall->dim.x = 800;
    wall->dim.y = 600;    
    game.objects.push_back(wall);
    
    Object* background = new Object();
    background->scene = GameSceneMenu;
    background->objectType = ObjectTypePlayer;
    background->color = Color(255, 0, 0);
    background->avgRadius = 100;
    background->pos.y = 50;
    background->pos.x = 150;
    background->dim.x = 300;
    background->dim.y = 500;    
    game.objects.push_back(background);
    
    
//    char buttons[][4] = {
//        "New Game",
//        "Restart",
//        "Exit",
//    };
//    for(int i = 0; i < buttons; ++i) {
//        Object* background = new Object();
//        background->scene = GameSceneMenu;
//        background->objectType = ObjectTypeRectangle;
//        background->color = Color(255, 255, 255, 255);
//        background->pos.y = 50;
//        background->pos.x = 100;
//        background->dim.x = 300;
//        background->dim.y = 500;    
//        game.objects.push_back(background);
//        Object* background = new Object();
//        background->scene = GameSceneMenu;
//        background->objectType = ObjectTypeText;
//        background->color = Color(255, 255, 255);
//        background->pos.y = 50;
//        background->pos.x = 100;   
//        game.objects.push_back(background);
//    }
    
}

void initScenePlay() {
    
}

void initScenePlayPause() {
    
}

void initSceneCredits() {
    
}

void renderAll() {    
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)    
        
    /**
     * If the game is paused, the background is still
     * rendered
     */
    if( game.scene == GameScenePlay || 
        game.scene == GameScenePlayPause) {
        renderMap();
    }
    
    //Rendered in order and let's hope it works
    for(int i = 0, l = game.objects.size(); i < l; ++i) {
        Object* obj = game.objects[i];
        if(game.scene == obj->scene) {            
            switch(obj->objectType) {
                case ObjectTypePlayer: 
                case ObjectTypeEnemy:
                    renderSphere(obj);
                break;                
                case ObjectTypeRectangle:
                    renderRectangle(obj);
                break;
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
    
    glXSwapBuffers(game.display, game.win);
}

void renderMap() {
    for(int x = 0; x < 800; ++x) {
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

    
void renderSphere(Object* obj) {
    int col = obj->color.toInt();        
    glBegin(GL_TRIANGLE_FAN);
    glColor4iv(&col);
    float   rad = obj->avgRadius * PIXEL_TO_METER,
            x = obj->pos.x,
            y = obj->pos.y;
        for (int i = 0; i < 360; ++i)
        {
           float degInRad = i*M_PI/180;
           glVertex2f(
                std::cos(degInRad)*rad + x,
                std::sin(degInRad)*rad + y
            );
        }
    glEnd();    
}

void renderRectangle(Object* obj) {
    GLint col = obj->color.toInt();  
    glColor4iv(&col);
    glRectf(
        obj->pos.x, 
        obj->pos.y, 
        obj->pos.x + obj->dim.x, 
        obj->pos.y + obj->dim.y
    );
}

void renderTexture(Object* sphere) {
    
}

void renderText(Object* obj) {
    Rect rect;
    rect.bot = obj->pos.y;
    rect.left = obj->pos.x;
    rect.center = 0;
    int cref = obj->color.toRGBInt();
    switch(obj->style) {
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
            ggprint13(&rect, 30,   cref, obj->name.c_str());
            break;
        case plain16:
            ggprint16(&rect, 30,   cref, obj->name.c_str());
            break;
        case plain40:
            ggprint40(&rect, 0,    cref, obj->name.c_str());
        break;
        default:
            ggprint06(&rect, 0, cref, obj->name.c_str());
            break;
    }
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
    return 500 + 10 * std::sin((x+ game.mapBoundsIteration*0.25)/25.0);
}

float getOceanFloorUpperBound(int x) {
    return 100 + 30 * std::sin(x/100.0);
}

float getOceanFloorLowerBound(int x) {
    return 0;
}






