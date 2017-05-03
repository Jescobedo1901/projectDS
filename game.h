
#ifndef GAME_H
#define GAME_H

//X11 and OpenGL headers
#include <cstdio>
#include <cstdlib>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

//C++ std library
#include <vector>
#include <cmath>
#include <sstream>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <climits>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <glob.h>
#include <ctime>

//other
#include "fonts.h"
#include "ppm.h"

const float PIXEL_TO_METER = 60.0f;

//3 Dimensional vector

struct Vec3 {
    Vec3();
    Vec3(float x, float y, float z);

    float x, y, z;

    float magnitude() const;

    //Create a normalized vector from this
    Vec3 norm() const;

    // += math operator
    Vec3& operator+=(const Vec3& r);

    // -+ math operator
    Vec3& operator-=(const Vec3& r);

    // *= math operator
    Vec3& operator*=(const Vec3& r);

};
//Simple math operators defined for Vec3
Vec3 operator*(const Vec3& v1, const Vec3& v2);
Vec3 operator*(const float& l, const Vec3& r);
Vec3 operator*(const Vec3& l, const float& r);
Vec3 operator/(const Vec3& l, const float& r);
Vec3 operator/(const float& l, const Vec3& r);
Vec3 operator+(const Vec3& v1, const Vec3& v2);
Vec3 operator+(const Vec3& l, const float& r);
Vec3 operator+(const float& l, const Vec3& r);
Vec3 operator-(const Vec3& v1, const Vec3& v2);
Vec3 operator-(const Vec3& l, const float& r);
Vec3 operator-(const float& l, const Vec3& r);
Vec3 operator-(const Vec3& v);
Vec3 abs(const Vec3& vec);
Vec3 sqrt(const Vec3& vec);
// Cross product between two vectors
Vec3 cross(const Vec3& l, const Vec3& r);


//Create typedefs Dimeinsion, Force, Position, Velocity, and Acceleration
//translated to simply Vec3, a vector class with
//simple math operations on x, y, and z values
typedef Vec3 Dimension;
typedef Vec3 Force;
typedef Vec3 Position;
typedef Vec3 Velocity;
typedef Vec3 Acceleration;



//ENUM declarations - BEGIN

//Color class, helpful to convert to int and RGB
//implemented in render.cpp

struct Color {
    Color();
    Color(unsigned char r, unsigned char g, unsigned char b);
    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    void glChangeColor();

    GLint toRGBInt(); //To RGB no alpha

    unsigned char r, g, b, a;
};

//Different object types
//used for handling physics and rendering

enum ObjectType {
    ObjectTypeInvalid,
    ObjectTypePlayer,
    ObjectTypeEnemy,
    ObjectTypeNeutral,
    ObjectTypeFriendly,
    ObjectTypeRectangle,
    ObjectTypeSphere,
    ObjectTypeTexture,
    ObjectTypeText
};

//Different rendering scenes
//used to control processing of physics and selective rendering

enum GameScene {
    GameSceneNone = 1,
    GameSceneMenu = 2,
    GameScenePlay = 4,
    GameSceneHUD = 8,
    GameScenePlayPause = 16,
    GameSceneCredits = 32,
    GameSceneHelp = 64,
    GameSceneUpgrades = 128,
    GameSceneLost = 256
};

enum TextStyle {
    plain6,
    plain7,
    plain8,
    bold8,
    plain10,
    plain12,
    plain13,
    plain16,
    plain17,
    plain40
};
//ENUM declarations - END

/*
 * Generic resource class
 */
struct Resource {

    Resource(std::string p);

    virtual ~Resource();

    virtual GLuint getResourceId() = 0;

    const std::string path;
};

/**
 * Single texture resource
 * @param texFile
 */
struct TextureResource : Resource {
    TextureResource(std::string texFile, int tol = 0);
    ~TextureResource();
    virtual GLuint getResourceId();
protected:
    Ppmimage *tex;
    GLuint texId;
    bool texTransUsingFirstPixel;
    int tolerance;
};

/**
 * Allows mapping multiple textures into a Flip Book
 * Composition of multiple TextureResource
 */
struct FlipBook : Resource {
    FlipBook(std::string pathWildcard, float fps, int tol = 0);
    ~FlipBook();
    void step(float stepDuration);
    virtual GLuint getResourceId();
protected:
    float duration;
    float fps;
    TextureResource* current;
    std::vector<TextureResource*> book;
};

struct Object {

    Object()
    : name(), objectType(), scene(GameSceneNone),
    pos(), vel(), acc(), avgRadius(),
    mass(), forces(0), color(), style(),
    dim(), offset(),
    intAttribute1(),
    resource(NULL)
    {
    }

    ~Object()
    {
    }


    //Name of this object (optional)
    std::string name;

    //Integer object type identifier (optional)
    ObjectType objectType;

    //The ID of the scene this object belongs to
    //By default, it does NOT belong to any scene
    GameScene scene;

    //The current position of this object
    Position pos;

    //The current velocity of this object
    Velocity vel;

    //the current acceleration of this object
    Acceleration acc;

    //The average radius of the object
    //Not necessarily a meaningful attribute
    float avgRadius;

    //The mass of the object (kg)
    float mass;

    // A vector of vector forces being applied to this object
    // Must be reapplied every step in order to have continuous effect
    //These forces added are per second
    std::vector<Force> forces;


    /**
     * (OPTIONAL)
     * The color of the object, if meaningful to object type
     */
    Color color;

    /**
     * (OPTIONAL)
     * The text style of this object, if meaningful to object type
     */
    TextStyle style;


    /**
     * (OPTIONAL)
     * The dimensions of this object, if meaningful to the object type
     */
    Dimension dim;

    /**
     * (OPTIONAL)
     * Positional offset, if meaningful to the object type, e.g. a texture
     */
    Position offset;

    /**
     * (OPTIONAL)
     * Optional integer attribute
     * If the type is ObjectTypeEenemy, then this is the amount of damage it inflicts
     */
    int intAttribute1;
	 /**
     * (OPTIONAL)
     * Optional Double attribute
     * Used for floats
     */
	double doubleAttribute1;

    Resource* resource;

    inline Force cumForces() const
    {
        Force cum;
        for (int i = 0, l = forces.size(); i < l; ++i) {
            cum.x += forces[i].x;
            cum.y += forces[i].y;
            cum.z += forces[i].z;
        }
        return cum;
    }
};

/**
 Score struct for player
 totalScore will hold aggregate score
 highestScore will hold max score in one play*/

struct Score {
    int totalScore;
    int highScore;
};

//Game struct
typedef std::map<std::string, Resource*> ResourceMap;

struct Game {
    //Initializes variables to null/default values
    //implementation defined in main
    Game();

    //X11 and OpenGL variables
    Display* display;
    Window root;
    Window win;
    XVisualInfo* vi;
    Colormap cmap;
    XSetWindowAttributes swa;
    XWindowAttributes gwa;
    GLXContext glc;
	
	
	clock_t start; //clock start of game


    //Set to true to exit the game
    bool done;


    /**
     * Controls which game flow we are currently in
     * e.g., menu, play, etc
     */
    int scene;
    bool isGamePaused;

    int lastButton;
	

    int mapBoundsIteration;

    /**
     * Keeps track of player keyboard directional input
     */
    int playerMovementDirectionMask;

    int xres;
    int yres;

    /**
     * Camera coordinates of the camera (default 0,0)
     * Automatically adjusted to center player during gamePlay
     * And not used for other scenes
     */
    Position camera;

    /**
     * The minimum x coordinate that the camera can show
     * Is incremented over time as the player moves right
     */
    float cameraXMin;

    /**
     * Game Objects
     * Every object is rendered depending on its
     * objectType attribute found in Object struct
     */
    std::vector<Object*> objects;

    /**
     * The player object instance, of type ObjectTypePlayer
     */
    Object* player;

    Object* healthTxt;

    Object* healthBar;

    Object* pointsTxt;
	
	Object* timeTxt;

    ResourceMap resourceMap;
    
    /**
     * Score struct for holding two score values for every player
     * See struct Score
     */
    std::vector<Score> score;
};

extern Game game;

//Physics section

enum Direction {
    DirLeft = 1,
    DirRight = 2,
    DirUp = 4,
    DirDown = 8,
    DirNone = 16
};

//Setup & Teardown
void initX11();
void initGL();
void initAudio();
void uninitX11();
void uninitGL();
void uninitAudio();

void uninitResources();

void initResources();
void initScenes();
void initSceneMenu();
void initScenePlay();
void initSceneHelp();
void initScenePlayPause();
void initSceneCredits();
void initSceneUpgrades();

//Core API
void gameLoop();
void elapsedTime();
void updateGameStats();

//Rendering+
void renderAll();
void renderMap();
void renderObjects(int scenesToRender);
void renderSphere(Object*);
void renderRectangle(Object*);
void renderTexture(Object*);
void renderText(Object*);
void generateFloorObjects(int x);

/**
 * This function maps a resource to an object
 * @param obj the object to map the resource to
 * @param resourceName
 */
void mapResource(Object* obj, const char* resourceName);

unsigned char *buildAlphaData(Ppmimage *,bool = true, int = 0);


//SkyBounds for rendering map
float getSkyUpperBound(int x);
float getSkyLowerBound(int x);
float getOceanUpperBound(int x);
float getOceanFloorUpperBound(int x);
float getOceanFloorLowerBound(int x);

//Events
void handleEvents();
void handlePlayerMovement(const XEvent& event);
void handlePlayerClickExit(const XEvent& event);
void handleMouseClicks(const XEvent& event);
void handleMenuMouseMovement(const XEvent& event);
void handleClickMenuItems(const XEvent& event);
void handleESC(const XEvent& event);


//Audio
void playHover();
void playClick();
void audioLoop();
void playPoint();
void playDmg();
void gameOver();
//Add audio handling functions here

//Physics
void stepPhysics(float stepDuration);
void stepMapBoundsIteration();
void stepFlipBooks(float stepDuration);

void applyNewtonianPhysics(Object* obj, float stepDuration);
void applyGravity(Object*);
void applyStokesApprox(Object*);
void applyBuoyancyApprox(Object*);
void applyPlayerMovement(Object*);
void applyPlayerDirChange(Object*);
void applySpawnRate(float stepDuration);
void applyPlayerOceanFloorCollision(Object*);
void spawnEnemy();
void spawnFriendly();

float dimToAvgRadius(Dimension dim);
float avgRadiusTOEstMass(float);

/**
 * Handle and detect player collisions
 * Implemented by Jacob
 */
void checkObjectCollisions();



/**
 * This function should call any object specific handlers to release resources
 * that are not handled manually, e.g. scrolling objects
 * @param
 */
bool applyObjectLifetimePolicies(Object*);

//Audio

inline void applyAudio()
{
}


//Error handling

inline void initFailure(const char* msg)
{
    printf("An initialization failure occured."
            "With the following message:\n\t%s\n", msg);
    std::exit(1);
}


#endif
