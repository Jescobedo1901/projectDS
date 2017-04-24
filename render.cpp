#include "game.h"
#include "ppm.h"
//GLOBAL PPM ARRAYS

GLuint playerTexture;
GLuint silhouetteTexture;
Ppmimage *playerImage = NULL;

//GLOBAL SCREEN SETUP
int xres = 800;
int yres = 600;

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
unsigned char *buildAlphaData(Ppmimage *img)
{
	//add 4th component to RGB stream...
	int i;
	int a,b,c;
	unsigned char *newdata, *ptr;
	unsigned char *data = (unsigned char *)img->data;
	newdata = (unsigned char *)malloc(img->width * img->height * 4);
	ptr = newdata;
	for (i=0; i<img->width * img->height * 3; i+=3) {
		a = *(data+0);
		b = *(data+1);
		c = *(data+2);
		*(ptr+0) = a;
		*(ptr+1) = b;
		*(ptr+2) = c;
		//get largest color component...
		//*(ptr+3) = (unsigned char)((
		//		(int)*(ptr+0) +
		//		(int)*(ptr+1) +
		//		(int)*(ptr+2)) / 3);
		//d = a;
		//if (b >= a && b >= c) d = b;
		//if (c >= a && c >= b) d = c;
		//*(ptr+3) = d;
		*(ptr+3) = (a|b|c);
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
            xres,
            yres,
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
    initSceneMenu();
    initScenePlay();
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
    screenBg->dim.x = xres;
    screenBg->dim.y = yres;
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
	glEnable(GL_TEXTURE_2D);

	Object* character = new Object();
	character->scene = GameScenePlay;
	character->name  = "player";
	character->objectType = ObjectTypePlayer;
	character->pos.y = 150;
	character->pos.x = 200;
	game.objects.push_back(character);
	//character->textureimage = ppm6GetImage("./images/bigfoot.ppm");
	playerImage = ppm6GetImage("./images/bigfoot.ppm");

	//BIND PPM DATA
	
	glGenTextures(1, &playerTexture);
	glGenTextures(1, &silhouetteTexture);
	//int w = character->textureimage->width;
	//int h = character->textureimage->height;
	int w = playerImage->width;
	int h = playerImage->height;
	glBindTexture(GL_TEXTURE_2D, playerTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

	//TRANSPARENCY

	unsigned char *sillhouetteData = buildAlphaData(playerImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			sillhouetteData);
	free(sillhouetteData);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			//playerImage->data);


	//Game Diagnostics Text
	Object* healthText = new Object();
	healthText->scene = GameScenePlay;
	healthText->objectType = ObjectTypeText;
	healthText->style = plain16;
	healthText->color = Color(244, 66, 66, 1);
	healthText->value = 100;
	healthText->pos.y = yres-50;
	healthText->pos.x = 0+50;
	healthText->name = (char)100;
	game.objects.push_back(healthText);

	Object* hunger = new Object();
	hunger->scene = GameScenePlay;
	hunger->objectType = ObjectTypeText;
	hunger->style = plain16;
	hunger->color = Color(255, 149, 50, 1);
	hunger->value = 100;
	hunger->pos.y = yres-50;
	hunger->pos.x = 0+150;
	hunger->name = (char)100;
	game.objects.push_back(hunger);

	Object* speed = new Object();
	speed->scene = GameScenePlay;
	speed->objectType = ObjectTypeText;
	speed->style = plain16;
	speed->color = Color(71, 244, 86, 1);
	speed->value = 0;
	speed->pos.y = yres-50;
	speed->pos.x = 0+250;
	speed->name = (char)100;
	game.objects.push_back(speed);
	


}

void initScenePlayPause()
{

}

void initSceneCredits()
{

}

void renderAll()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)

    /**
     * If the game is paused, the background is still
     * rendered
     */
    if (game.scene == GameScenePlay ||
            game.scene == GameScenePlayPause) {
	
        renderMap();
	
	
    }
	
    //Rendered in order and let's hope it works
    for (int i = 0, l = game.objects.size(); i < l; ++i) {
        Object* obj = game.objects[i];
        if (game.scene == obj->scene) {
            switch (obj->objectType) {
            case ObjectTypePlayer: 
		renderCharacter(obj);
		break;
            case ObjectTypeEnemy:
                renderSphere(obj);
                break;
            case ObjectTypeSphere:
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

void renderMap()
{
    for (int x = 0; x < 800; ++x) {
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

void renderCharacter(Object* obj)
{
	
	glColor4f(1.0,1.0,1.0,0.8f);
	
	
	
	glPushMatrix();
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.0f);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, playerTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(1.0f,1.0f); glVertex2i(obj->pos.x,obj->pos.x);
	glTexCoord2f(1.0f,0.0f); glVertex2i(obj->pos.x,(obj->pos.y)+100);
	glTexCoord2f(0.0f,0.0f); glVertex2i((obj->pos.x)+100, (obj->pos.y)+100);
	glTexCoord2f(0.0f,1.0f); glVertex2i((obj->pos.x)+100, obj->pos.x);
	glEnd();
	glPopMatrix();
	
}

void renderTexture(Object* obj)
{
	
}

void renderText(Object* obj)
{
    Rect rect;
    rect.bot = obj->pos.y;
    rect.left = obj->pos.x;
    rect.center = 0;
    int cref = obj->color.toRGBInt();
    if(obj->value != NULL)
	{
		char str[5];
		sprintf(str, "%d", obj->value);
		
	switch (obj->style) {
    case plain6:
        ggprint06(&rect, 0, cref, str);
        break;
    case plain7:
        ggprint07(&rect, 0, cref, str);
        break;
    case plain8:
        ggprint08(&rect, 0, cref, str);
        break;
    case bold8:
        ggprint8b(&rect, 0, cref, str);
        break;
    case plain10:
        ggprint10(&rect, 0, cref, str);
        break;
    case plain12:
        ggprint12(&rect, 0, cref, str);
        break;
    case plain13:
        ggprint13(&rect, 30, cref, str);
        break;
    case plain16:
        ggprint16(&rect, 30, cref, str);
        break;
    case plain40:
        ggprint40(&rect, 0, cref, str);
        break;
    default:
        ggprint06(&rect, 0, cref, str);
        break;
    }
	}
    else{
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






