//Omar Thor
//Group 4
//DeepSea Survival Game

//Include headers for networking
#include <sys/socket.h>
#include <netdb.h>

#include "game.h"

/**
 * The following functions of the game are implemented in this source file:
 *
 * Resource generation
 *      void decompressedResourcesCleanup()
 *      void decompressResources()
 * Scene loading & rendering
 *      void renderAll()
 * Generation of the game's map:
 *      void renderMap()
 *      float getSkyUpperBound(int x)
 *      float getSkyLowerBound(int x)
 *      float getOceanUpperBound(int x)
 *      float getOceanFloorUpperBound(int x)
 *      float getOceanFloorLowerBound(int x)
 *      void stepMapBoundsIteration()
 * Physics Handling:
 *      void stepPhysics(float stepDuration)
 *      void applyNewtonianPhysics(Object* obj, float stepDuration)
 *      void applyBuoyancyApprox(Object* obj)
 *      void applyGravity(Object* obj)
 *      void applyStokesApprox(Object* obj)
 *      void applyPlayerMovement(Object* obj)
 *      void handlePlayerMovement(const XEvent& event)
 *      void applyObjectBoundaryCollision(Object* obj)
 *      void applyPlayerOceanBurstMovement(Object* player, float stepDuration)
 *      bool isOffscreen(Object* obj)
 *      bool applyObjectLifetimePolicies(Object* obj)
 *      void applyNonPlayerMotion(Object* hobj, float stepDuration)
 *      void applyRotationalHandling(Object* obj, float stepDuration)
 * Resource classes / image handling
 *      Resource::Resource(std::string p)
 *      Resource::~Resource()
 *      TextureResource::TextureResource(std::string texFile, int tol)
 *      GLuint TextureResource::getResourceId()
 *      FlipBook::FlipBook(std::string, float, int)
 *      FlipBook::~FlipBook()
 *      void FlipBook::step(float stepDuration)
 *      GLuint FlipBook::getResourceId()
 *      void FlipBook::setFPS(float newFPS)
 *      void mapResource(Object* obj, const char* resourceName)
 *      void stepFlipBooks(float stepDuration)
 *      unsigned char *addAlphaChannel(Ppmimage *img, bool firstPixel, int tol)
 * High score / networking:
 *      int updateHighScores(std::string username, int latestHigh)
 * Game logic
 *      void updateGameStats()
 * Keyboard input halding
 *      void handleLoginInput(const XEvent& event)
 *
 *
 * Note: This file contains specific sections which were my personal
 * responsibility of maintaining and implementing. However, this file does not
 * represent my total contribution to this project.
 */

/**
 * Automatically cleanup all ppm images from the ./images directory (generated)
 *
 * This function uses the opendir, readdir, and remove functions
 */
void decompressedResourcesCleanup()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./images");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                std::string ext(".ppm"), texFile(dir->d_name);
                //Verify that the possible texture file (texFile)
                //ends with .ppm
                if (texFile.size() > ext.size() &&
                        texFile.compare(
                        texFile.size() - ext.size(), ext.size(), ext
                        ) == 0) {
                    std::string removingFile = std::string("./images/") +
                            dir->d_name;
                    if (remove(removingFile.c_str()) == -1) {
                        perror("Error deleting temporary resource file");
                    } else {
                        printf(
                            "Resource: %s - cleaned up\n",
                            removingFile.c_str()
                        );
                    }
                }
            }
        }
        closedir(d);
    }
}

/**
 * Decompress all resources (images)
 *
 * This function will read all non-ppm images in the images directory and
 * convert them to ppm format usable by the ppm header
 */
void decompressResources()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./images");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                std::string ext(".ppm"), texFile(dir->d_name);
                //Select only files that do not end with .ppm
                if (texFile.size() > ext.size() &&
                        (texFile.compare(
                        texFile.size() - ext.size(), ext.size(), ext
                        ) != 0 &&
                        texFile.find_first_of(".") != 0)) {
                    std::string newFile = "./images/" + texFile + ext;
                    std::string command =
                            "convert \"./images/" + texFile + "\" \"" +
                            newFile + "\"";
                    int res = system(command.c_str());
                    if (WIFEXITED(res) && WEXITSTATUS(res) == 0) {
                        printf("Resource: %s - generated\n", newFile.c_str());
                    }
                }
            }
        }
        closedir(d);
    }
}

/**
 * This function is responsible for the rendering logic and keeping track
 * of the motion of the "camera" as the player moves on the screen.
 *
 */
void renderAll()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    //When game scene play is loaded  or the game is paused
    if (game.scene & GameScenePlay || game.isGamePaused) {
        glPushMatrix();
        //Limit camera X min boundary to half the the left screen
        //of the player but do not allow player to go back
        //more than half the screen
        game.cameraXMin = std::max(
            game.cameraXMin, game.player->pos.x - game.xres / 2
        );
        //This moves the camera x min forward when the player moves forward
        game.camera.x = std::max(
            game.cameraXMin, game.player->pos.x - game.xres / 4
        );
        //Move the camera with the player movion
        glTranslatef(-game.camera.x, -game.camera.y, -game.camera.z);
        renderMap();
        renderObjects(GameScenePlay);
        glPopMatrix();
        if (game.scene & GameSceneHUD) {
            renderObjects(GameSceneHUD);
        }
    }
    //If the game scene play and hud are NOT loaded, make sure
    //to render the map in the background
    if (game.scene & ~(GameScenePlay | GameSceneHUD)) {
        renderMap();
        renderObjects(game.scene);
    }

    glXSwapBuffers(game.display, game.win);
}

/**
 * Render the map onto the screen.
 *
 * To keep things simple, the map is rendered inplace of the game's translation
 * This means that we only need to draw the map on the screen where the player
 * is located
 *
 * @TODO replace vertical line drawing with vertices, to remove occasional
 * effect where the blurring of the lines becomes visible if the player
 * is moving too fast
 */
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

//Map generation parameter functions

/**
 * This function defines the upper boundary of the sky (y resolution, constant)
 */
float getSkyUpperBound(int x)
{
    return game.yres;
}

/**
 * Returns the definition of the sky's lower
 * boundary (75% of y resolution, constant)
 */
float getSkyLowerBound(int x)
{
    return (.75 * game.yres);
}

/**
 * Returns the definition of the ocean's upper boundary as a sine wave
 * modeled as a function of time (game.mapBoundsIteration)
 *
 * This function will make the ocean move to the left even if the player
 * is not moving due to the time parameter of mapBoundsIteration
 * which is updated when the game is player
 *
 * The parameters of this function were defined experimentally to provide
 * for a simple but effective "waves"
 *
 */
float getOceanUpperBound(int x)
{
    return (.8 * game.yres) + 10 *
            std::sin((x + game.mapBoundsIteration * 0.25) / 25.0);
}

/**
 * Returns the definition of the ocean floor upper bound
 * as a function of a sine wave which is constant for x,
 * meaning that the function does not move, only the player.
 * (surface moves left when player moves right)
 */
float getOceanFloorUpperBound(int x)
{
    return 100 + 30 * std::sin(x / 100.0);
}

/**
 * Returns the definition of the ocean floor lower bound (screen bottom)
 * @param x
 * @return
 */
float getOceanFloorLowerBound(int x)
{
    return 0;
}

/**
 * Increments map bounds iteration such that
 * the ocean can move forward in time based on the rate at which
 * the physics calculation are run
 */
void stepMapBoundsIteration()
{
    ++game.mapBoundsIteration;
}

/**
 * Moves the physics of the game play forward in time by
 * the specified stepDuration parameter
 *
 * It is only applied if the gameScene play is loaded
 */
void stepPhysics(float stepDuration)
{
    /**
     * No physical effects are apply
     * unless the game scene loaded is play
     */
    if (game.scene & GameScenePlay) {

        //Move map generation forward
        stepMapBoundsIteration();

        applySpawnRate(stepDuration);

        stepFlipBooks(stepDuration);

        /**
         * Loop through all objects and apply the correct
         * physics effects depending on the type of the object
         */
        for (std::vector<Object*>::iterator
            it = game.objects.begin(),
            end = game.objects.end();
            it != end;) {
            Object* obj = *it;
            //Check if object has reached the end of its life
            if (applyObjectLifetimePolicies(obj)) {
                it = game.objects.erase(it);
                continue;
            }
            //Apply to all except object type enemy
            switch (obj->objectType) {
            case ObjectTypeEnemy:
                //Specifically apply gravity to enemy3 types
                if (obj->name == "enemy3") {
                    applyGravity(obj);
                    break;
                } else if (obj->name == "ship" ||
                        obj->name == "ship-wreck") {
                    //circumvent non player motion and apply only default
                    //effects on this object
                    goto handleShip;
                }
            case ObjectTypeFriendly:
            case ObjectTypeNeutral:
                //Friendlies and neutrals have specific motion
                //defined in applyNonPlayerMotion
                applyNonPlayerMotion(obj, stepDuration);
                break; //Just break, don't apply the below
            handleShip: //
            default:
                applyGravity(obj);
                applyStokesApprox(obj);
                applyBuoyancyApprox(obj);
                break;
            }
            applyNewtonianPhysics(obj, stepDuration);
            switch (obj->objectType) {
            case ObjectTypePlayer:
                /**
                 * The following physics effects are
                 * applied specifically to player ONLY
                 */
                applyPlayerMovement(obj);
                applyPlayerDirChange(obj);
                applyPlayerOceanBurstMovement(obj, stepDuration);
                break;
            default:
                break;
            }

            applyRotationalHandling(obj, stepDuration);
            applyObjectBoundaryCollision(obj);
            ++it;
        }
        /**
         * Check all objects for collisions
         * This function is the responsibility of Jacob
         * and the implementation can be found in jacobE.cpp
         */
        checkObjectCollisions();
    }
}

/**
 * Translate all physical forces into a single vector
 * and calculate the acceleration and apply it is a
 * fraction of a second (stepDuration) to the velocity
 * which then is added to the position as a fraction of
 * a second (stepDuration)
 *
 * Note: Only objects with a non-zero mass are considered
 * for physical forces
 *
 * Equations used:
 * F = ma
 * a = F / m
 * p = v t
 */
void applyNewtonianPhysics(Object* obj, float stepDuration)
{
    Vec3 force = obj->cumForces();
    obj->forces.clear(); //clear applied forces

    //Apply only to objects with mass
    if (obj->mass) {
        // a = F / m
        obj->acc = force / obj->mass;

        //Velocity  = acceleration * time
        obj->vel += obj->acc * stepDuration;

        //Position = velocity * time
        // Adjust pixel to meter radio
        obj->pos += obj->vel * stepDuration * PIXEL_TO_METER;
    }
}

/**
 * Apply force of buoyancy for any object under the waterline
 * as long as it has a mass
 *
 *
 * Note: This function estimated volume based on the object's
 * average radious property
 *
 *
 * Equations used:

 *
 * Force of buoyancy = Vol * g * μ_water
 * Volume = 4/3 r ^3 PI
 *
 * Constants:
 *  μ_water = 1
 */
void applyBuoyancyApprox(Object* obj)
{
    //Only apply Buoyancy approximation to objects that have mass
    //and if below waves
    int waterLine = getOceanUpperBound(obj->pos.x);
    if (obj->mass &&
            obj->pos.y <= waterLine) {

        //Water Density
        float waterDensity = 1;

        float radius = obj->avgRadius;
        float volEst
                = 4.0 / 3.0 * radius * radius * radius * M_PI;

        //Force of Gravity
        Acceleration forceG(0, -9.8, 0);
        //F_B = V * D * F
        Vec3 forceOfBuoyancy = -(volEst * forceG * waterDensity);
        obj->forces.push_back(forceOfBuoyancy);
    }
}

/**
 * Gravity is applied to all objects that have mass uniformly (negative y)
 *
 * Equations used:
 * Force of gravity = ma => Force of gravity y - axis = -9.8 * m
 */
void applyGravity(Object* obj)
{
    //Apply gravity on objects with mass
    if (obj->mass > 0) {
        obj->forces.push_back(obj->mass * Acceleration(0, -9.8, 0));
    }
}

/**
 * The stokes approximation provides for a realistic motion in water
 * where the volume of the object is considered. Only objects under
 * the water line are considered and only ones that have a non-zero mass.
 *
 * This was an area of research where the following sources were eventually
 * used to aid in the formation of a "good enough" approximation to model
 * drag under water:
 * http://hyperphysics.phy-astr.gsu.edu/hbase/lindrg.html#c2
 * https://en.wikipedia.org/wiki/Drag_(physics)
 * https://en.wikipedia.org/wiki/Stokes'_law
 */
void applyStokesApprox(Object* obj)
{

    //Only apply Stokes approximation to objects that have mass
    //and if below waves
    if (obj->mass &&
            obj->pos.y <= getOceanUpperBound(obj->pos.x)) {
        //Viscoscity of water at 20C
        float water_mu = 1.002;
        //radius
        float r = obj->avgRadius;
        Vec3 force = -(6.0f * (float) (M_PI) * water_mu * r * obj->vel);

        obj->forces.push_back(force);
    }
}

/**
 * Applies player's input to the player object only when the player object
 * is under water. This is so that the player loses control when jumping
 * out of the water providing for a more realistic game play.
 *
 * Note: It is possible to apply forces to two directions at a the same time.
 * Initially, this was merely an oversight but eventually we decided to keep
 * this method as it allows the player to move at angles faster than in
 * straight lines, as it seems to have a positive effect in that players
 * are encourages to move up and down as they move forward.
 *
 * @param obj
 */
void applyPlayerMovement(Object* obj)
{
    if (obj->pos.y < getOceanUpperBound(obj->pos.x)) {
        float thrust = game.thrustModifier * obj->mass;
        if (game.playerMovementDirectionMask & DirUp)
            obj->forces.push_back(Vec3(0, thrust, 0));
        if (game.playerMovementDirectionMask & DirDown) {
            obj->forces.push_back(Vec3(0, -(thrust), 0));
        }
        if (game.playerMovementDirectionMask & DirRight) {
            obj->forces.push_back(Vec3(thrust, 0, 0));
        }
        if (game.playerMovementDirectionMask & DirLeft) {
            obj->forces.push_back(Vec3(-(thrust), 0, 0));
        }
    }
}

/**
 * Listens to play inputs and modified the player movement mask using bit
 * operators. On keyPress, the bit mask for the direction is added,
 * on KeyRelease, the bit mask for the direction is removed from the player
 * movement mask.
 *
 * See: applyPlayerMovement(...)
 */
void handlePlayerMovement(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up || key == XK_w) {
            game.playerMovementDirectionMask |= DirUp;
        }
        if (key == XK_Down || key == XK_s) {
            game.playerMovementDirectionMask |= DirDown;
        }
        if (key == XK_Left || key == XK_a) {
            game.playerMovementDirectionMask |= DirLeft;
        }
        if (key == XK_Right || key == XK_d) {
            game.playerMovementDirectionMask |= DirRight;
        }
    } else if (event.type == KeyRelease) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up || key == XK_w) {
            game.playerMovementDirectionMask &= ~DirUp;
        }
        if (key == XK_Down || key == XK_s) {
            game.playerMovementDirectionMask &= ~DirDown;
        }
        if (key == XK_Left || key == XK_a) {
            game.playerMovementDirectionMask &= ~DirLeft;
        }
        if (key == XK_Right || key == XK_d) {
            game.playerMovementDirectionMask &= ~DirRight;
        }
    }
}

/**
 * Makes sure the player can not move past the screen
 * more than the minimum allowable position of the camera.
 *
 * If the player attempts to "collide" into the left side of the screen,
 * he simply bounces back.
 *
 * Additionally, the function modified the way the treasure object
 * is located on the y axis as we wanted it to rest slightly below
 * the bottom of the ocean floor upper boundary
 */
void applyObjectBoundaryCollision(Object* obj)
{
    switch (obj->objectType) {
    case ObjectTypePlayer:
        //Only let player move left left from position game.xres/2
        if (obj->pos.x < game.cameraXMin) {
            obj->pos.x = game.cameraXMin;
            obj->vel.x = -obj->vel.x;
        }
    case ObjectTypeEnemy:
    case ObjectTypeFriendly:
        if (obj->name != "treasure" &&
            obj->pos.y < (  getOceanFloorUpperBound(obj->pos.x) +
                            obj->avgRadius * PIXEL_TO_METER)) {
            obj->pos.y = (  getOceanFloorUpperBound(obj->pos.x) +
                            obj->avgRadius * PIXEL_TO_METER);
        }
    default:
        break;
    }
}

/**
 * This function caps the maximum velocity that the player
 * can escape from the ocean. We found that after several upgrades, the
 * velocity became so great that the player was in the air for such a long
 * time that it was detrimental to the game play, we simply capped the vertical
 * velocity at a highest value of 8.0
 *
 */
void applyPlayerOceanBurstMovement(Object* player, float stepDuration)
{
    if (    player->pos.y > getOceanUpperBound(player->pos.x) &&
            player->vel.y > 8.0f) {
        player->vel.y = 8.0f;
    }
}

/**
 * Helper function for the function applyObjectLifetimePolicies to help
 * determine whether or not the object is off screen.
 */
bool isOffscreen(Object* obj)
{
    return obj->pos.x <= game.player->pos.x - game.xres * 0.5;
}

/**
 * Applies and determines whether or not an object lifetime has expired
 * It considers only friendlies, neutrals and enemies for termination.
 * In the case where an object is determined to be off screen, it is
 * deleted, and it returns true as a signal that it can be safely
 * removed from the vector of objects.
 *
 */
bool applyObjectLifetimePolicies(Object* obj)
{
    switch (obj->objectType) {
    case ObjectTypeFriendly:
    case ObjectTypeNeutral:
    case ObjectTypeEnemy:
        if (isOffscreen(obj)) {
            delete obj;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

/**
 * The following functions are a part of the Resource base class,
 * its prototype is as follows:
 *
 * struct Resource {
 *   Resource(std::string p);
 *   virtual ~Resource();
 *   virtual GLuint getResourceId() = 0;
 * };
 */

/**
 * Base constructor of Resource class
 * @param p the name or the path of the resource
 */
Resource::Resource(std::string p)
: path(p)
{
}

/**
 * Resource base class must have a virtual destructor
 */
Resource::~Resource()
{
}

/**
 * TextureResource is a subclass of Resource
 * It converts an image file to a openGL texture which can then be
 * easily reused throughout the program
 *
 * @param texFile the name of the texture file (with or without ppm extension
 * as it will automatically be converted to the appropriate ppm extension
 * @param tol the tolerance of the pixel translation which allows
 * the addAlphaChannel function to have a tolerance of how close the color is.
 * See addAlphaChannel for details.
 */
TextureResource::TextureResource(std::string texFile, int tol)
    :   Resource(texFile),
        texId(),
        texTransUsingFirstPixel(true),
        tolerance(tol)
{
    glEnable(GL_TEXTURE_2D);
    //If textureFile does not end with .ppm,
    //we must map it to the generated .ppm first
    std::string ext(".ppm");
    Ppmimage* tex = NULL;
    if (texFile.size() > ext.size() &&
            texFile.compare(texFile.size() - ext.size(), ext.size(), ext)
            != 0) {
        std::string mappedFile = texFile + ext;
        tex = ppm6GetImage(mappedFile.c_str());
    } else {
        tex = ppm6GetImage(texFile.c_str());
    }
    glGenTextures(1, &this->texId);
    int w = tex->width;
    int h = tex->height;
    glBindTexture(GL_TEXTURE_2D, this->texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //TRANSPARENCY
    unsigned char *texAlphaData = addAlphaChannel(
        tex,
        this->texTransUsingFirstPixel,
        this->tolerance
    );
    glTexImage2D(
        GL_TEXTURE_2D, 0,
        GL_RGBA, w, h, 0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        texAlphaData
    );
    ppm6CleanupImage(tex);
    free(texAlphaData);
    glDisable(GL_TEXTURE_2D);
}

/**
 * Returns the resource identifier for using the texture in openGL
 */
GLuint TextureResource::getResourceId()
{
    return this->texId;
}

/**
 * The following functions are a part of the FlipBook class which is a sub
 * class of the Resource base class and have a composition of TextureResources
 * of one to many.
 *
 * The prototype for FlipBook defined as follows:
 *
 * struct FlipBook : Resource {
 *      FlipBook(std::string pathWildcard, float fps, int tol = 0);
 *      ~FlipBook();
 *      void step(float stepDuration);
 *      virtual GLuint getResourceId();
 *      virtual void setFPS(float fps);
 *  protected:
 *      float duration;
 *      float fps;
 *      TextureResource* current;
 *      std::vector<TextureResource*> book;
 * };
 */
/**
 *
 * The constructor for the FlipBook resource class.
 *
 * A FlipBook maps multiple TextureResources as a single Resource.
 * Then it provides for a way flipping through the pages, at a constant
 * rate (that is framesPerSecond, or FPS). This frame rate is used to determine
 * which texture it should be selecting at any given time.
 *
 * FlipBook also supports a way of adjust FPS on the fly, which is the way that
 * the player's fish animation is speedup the faster the player is moving.
 *
 * @param pathWildcard a wildcard path such as "./images/coin*" that will
 * be expanded to contain all textures that match the filename
 *
 * @param framesPerSeconds the number of frames to display per second
 *
 * @param tolerance this parameter will be passed to the TextureResource
 * classes created as a result of the initialization of this class
 */
FlipBook::FlipBook(
        std::string pathWildcard,
        float framesPerSeconds,
        int tolerance)
: Resource(pathWildcard), duration(), fps(framesPerSeconds),
current(NULL), book()
{
    //find matching textures file with glob
    glob_t results;
    if (glob(pathWildcard.c_str(), 0, NULL, & results) != 0) {
        initFailure(("Failed globbing textures for: " + pathWildcard).c_str());
    } else {
        for (size_t i = 0; i < results.gl_pathc; i++) {
            this->book.push_back(
                new TextureResource(results.gl_pathv[i], tolerance)
            );
        }
        globfree(& results);
        if (this->book.size() > 0) {
            this->current = this->book[0];
        }
    }
}

/**
 * Deconstructor of the FlipBook simply iterates over all the pages
 * in the book and deletes them, calling their own destructors.
 */
FlipBook::~FlipBook()
{
    //free resources
    for (std::vector<TextureResource*>::iterator
        it = book.begin(),
            end = book.end();
            it != end; ++it) {
        delete *it;
    }
}

/**
 * This function is called on a per FlipBook basis as a way
 * of altering each FlipBook's FPS independently of other animations
 * This function also selects which frame should be displayed at any given
 * time.
 *
 * @param stepDuration the amount the game timer has moved forward
 */
void FlipBook::step(float stepDuration)
{
    this->duration += stepDuration;
    int frame = (int(this->duration * this->fps)) % this->book.size();
    this->current = this->book[frame];
}

/**
 * Returns the currently selected page in the FlipBook. See step(...) function
 * to see how we decide which frame to display.
 */
GLuint FlipBook::getResourceId()
{
    return this->current->getResourceId();
}

/**
 * Adjust the FPS of the FlipBook, this can be done on the fly to
 * change the number of frames to display per second
 *
 * @param newFPS the new frame rate per second
 */
void FlipBook::setFPS(float newFPS)
{
    this->duration = this->duration * this->fps / newFPS;
    this->fps = newFPS;
}

/**
 * Map a resource by name to an object. Makes it easier to find the appropriate
 * resource to be used by the object. It searches (using a binary search) a map
 * of all resource names to their respective Resource representation
 *
 * @param obj the object to map the resource to
 * @param resourceName the name of the resource to map to the object
 */
void mapResource(Object* obj, const char* resourceName)
{
    ResourceMap::iterator it = game.resourceMap.find(resourceName);
    if (it != game.resourceMap.end()) {
        obj->resource = (*it).second;
    } else {
        initFailure(
            (std::string("Resource not found: ") + resourceName).c_str()
        );
    }
}

/**
 * This function is called in stepPhysics to
 * iterate over all resources, and check for an instance of a FlipBook,
 * if a resource is a FlipBook, it's internal step counter is increment
 * by calling the FlipBook.step(...) function
 *
 * @param stepDuration the amount of time the game time has moved forward
 */
void stepFlipBooks(float stepDuration)
{
    for (ResourceMap::iterator it = game.resourceMap.begin(),
            end = game.resourceMap.end();
            it != end;
            ++it) {
        FlipBook* fp = dynamic_cast<FlipBook*> ((*it).second);
        if (fp) {
            fp->step(stepDuration);
        }
    }


}

/**
 * Builds transparency texture using non-transparent image data
 *
 * This function builds ontop of Gordon's original buildAlphaData function
 * but adds a way of using the first pixel of the image to automatically
 * infer the color which should be changed to transparency and it also
 * provides for a way of adding tolerance to the way a pixel is determined
 * whether or not it is close enough to the transparency color
 *
 * The reason for this tolerance parameter is that due to compression
 * necessary to keep the size of the images down, it introduces pixelation of
 * plain background color which meant that sharp images were no longer
 * as sharp as before. This simple measure avoids that problem.
 *
 * @param img the Ppmimage image to process
 * @param firstPixel If true, it uses the first pixel to determine the
 * color that is transparent color
 * @param tol the tolerance of that pixel, or how close the pixel can be to
 * the firstPixel color to be considered transparent
 * @return a new object allocated by malloc, therefore must be deallocated with
 * free(...)
 */
unsigned char *addAlphaChannel(
        Ppmimage *img,
        bool firstPixel,
        int tol)
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
        *(ptr + 3) = !( std::abs(a - ta) <= tol &&
                        std::abs(b - tb) <= tol &&
                        std::abs(c - tc) <= tol );
        ptr += 4;
        data += 3;
    }
    return newdata;
}

/**
 * Updates the high scores on the server, retrieves the list of the player's
 * scores, and updates the high score list on the client
 *
 * The server will send back a list of 11 high score entries, 10 of which
 * are the global high scores for all players, and the last entry is the
 * player who called this function. This provides a way to "resume" game play
 * using the provided username. Of course, for simplicity's sake, we did not
 * provide a way to assign a user a password.
 *
 * Method: Connects to sleipnir.cs.csub.edu web server and does a get
 * request (HTTP  - GET) to the web server, submitting the username
 * and the latestHigh (possibly a 0) as URL parameters.
 *
 * @param username The username entered on startup
 * @param latestHigh the last high score, a score to be published.
 * @return 0 on success, -1 on failure
 */
int updateHighScores(std::string username, int latestHigh)
{
    const char* host = "sleipnir.cs.csub.edu";
    const char* protocol = "http";
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(host, protocol, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)
                ) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no connection
        fprintf(stderr, "failed to connect\n");
        return -1;
    }

    const char* page = "/~oomarsson/3350/server.php?username=";
    const char* useragent = "projectDS-BETA";

    std::stringstream ss;
    ss << "GET " << page << username
            << "&score=" << latestHigh
            << " HTTP/1.0\r\nHost: " << host <<
            "\r\nUser-Agent: " << useragent << "\r\n\r\n";

    std::string request = ss.str();
    const char * requestGet = request.c_str();

    int sent = 0;
    int getReqSize = strlen(requestGet);

    while (sent < getReqSize) {
        int sentBytes = write(sockfd, requestGet, strlen(requestGet));
        if (sentBytes == -1) {
            herror("Failed sending data");
            return -1;
        }
        sent += getReqSize;
    }

    std::string result;
    char buffer[1024 * 32] = {0};
    int bytesRead = -1;

    do {
        bytesRead = recv(sockfd, buffer, sizeof (buffer) - 1, 0);
        if (bytesRead > 0) {
            result += buffer;
            bzero(buffer, sizeof (buffer));
        }
    } while (bytesRead > 0);

    unsigned int subidx = result.find("\r\n\r\n") + 4;

    if (subidx < result.size()) {
        result = result.substr(subidx);
        ss.str("");
        ss << result;
        std::string user;
        int total, max;

        std::vector<Score> scores;

        while (ss >> user >> total >> max) {
            scores.push_back(Score(user, total, max));
        }

        if (!scores.empty()) {
            Score last = *(scores.begin() + scores.size() - 1);
            scores.erase(scores.end());
            game.playerInfo.highScore = last.highScore;
            game.playerInfo.totalScore = last.totalScore;
        }

        std::vector<Score>::iterator scoresIt = scores.begin();

        //Copy results into objects for rendering
        //Traverse through all objects in reverse/
        //find all objects of type text and on game scene score
        //then set them one by one to the values retrieved
        std::stringstream ss;
        for (int i = game.scoreObjects.size() - 1; i >= 0; --i) {
            if (scoresIt != scores.end()) {
                std::vector<Object*>& vec = game.scoreObjects[i];
                Score& s = *scoresIt;
                ss.str("");
                ss << s.name;
                vec[0]->name = ss.str();

                ss.str("");
                ss << s.highScore;
                vec[1]->name = ss.str();
                ss.str("");
                ss << s.totalScore;
                vec[2]->name = ss.str();

                ++scoresIt;
            }
        }
    } else {
        printf("Error seeking data");
    }

    close(sockfd);
    freeaddrinfo(servinfo);

    return 0;
}

/**
 * Apply a randomized sinusoidal player motion
 * It initializes the object doubleAttribute1 to a random value
 * and then maps a sine wave to the ocean floor upper boundary
 * and the ocean upper boundary.
 *
 * Note: This function could use more improvement, but after attempting many
 * ways of randomizing the motion, this is still the one that provides the
 * best game play.
 *
 * @param obj the object to be considered for non-player motion
 * @param stepDuration how much the game time has moved forward
 */
void applyNonPlayerMotion(Object* obj, float stepDuration)
{
    if (obj->objectType == ObjectTypeEnemy) {
        if (obj->doubleAttribute1 == 0) {
            //If double attribute is not set, see the motion pattern here
            obj->doubleAttribute1 = ((float) rand() / (float) RAND_MAX + 0.5f);
        }

        float ymin = getOceanFloorUpperBound(obj->pos.x);
        float ymax = getOceanUpperBound(obj->pos.x);

        float time = obj->pos.x;
        float phase = 0.0f;
        float freq = std::min(
            0.025f * stepDuration * obj->doubleAttribute1, 1.0 / 750.0
        );
        float ampl = ((ymax - ymin) / 2.0f);
        float shift = ymax / 2.0f;

        obj->pos.y = ampl * std::sin(2 * M_PI * freq * time + phase) + shift;
    }
}

/**
 * This function handles ALL rotation logic
 *
 * It provides two methods of rotation:
 *
 * 1) Setting object's rotateByVelocity - This method uses the velocity as a means of
 * determining which direction the object should be rotating, but it is fixed
 * at a 45% downward or upward angle at most.
 *
 * 2) Setting object's rotationTarget, fixedRotation, and rotationRate, an object
 * can be rotated by a fixed amount per second, which is the method by which
 * the pirate ship is rotated as it sinks down. For this method to take effect,
 * rotationTarget must not be equal to the objects rotation, and fixedRotation
 * must be set to true, then the object will be rotated by rotationRate
 * (degrees) per second
 *
 * @param obj the object to consider for rotation handling
 * @param stepDuration the time the game play has moved forward
 */
void applyRotationalHandling(Object* obj, float stepDuration)
{
    if (obj->rotateByVelocity) {
        float deg = obj->vel.angleXY();
        float target = 0.0f;
        if (obj->vel.x > 0) {
            if (obj->vel.y > 0) {
                target = std::min(deg * 180 / M_PI, 45.0);
            } else {
                target = std::max(deg * 180 / M_PI, -45.0);
            }
        } else if (obj->vel.x < 0) {
            if (obj->vel.y > 0) {
                target = std::min((deg + M_PI) * 180 / M_PI, -45.0);
            } else {
                target = std::min((deg + M_PI) * 180 / M_PI, 45.0);
            }
        }
        if (obj->fixedRotation) {
            obj->rotation += target * stepDuration * obj->rotationRate;
        } else {
            obj->rotation = target;
        }
    } else if (obj->rotationTarget != obj->rotation && obj->fixedRotation) {
        float delta = stepDuration * obj->rotationRate;
        if (    obj->rotationRate > 0 &&
                obj->rotation < obj->rotationTarget) {
            obj->rotation = std::min(
                obj->rotation + delta, obj->rotationTarget
            );
        } else if ( obj->rotationRate < 0 &&
                    obj->rotation > obj->rotationTarget) {
            obj->rotation = std::max(
                obj->rotation + delta, obj->rotationTarget
            );
        }
        printf("Rotating object: %f\n", delta);
    }
}

/**
 * Updates textual information in the game and provides a terminal play
 * condition where the game state is reset in the case of the player
 * depleting all health points.
 *
 */
void updateGameStats()
{
    //Link the health bar to the health text int attribute
    if (game.healthTxt->intAttribute1 <= 0) {
        audioLoop();
        gameOver();
        game.scene = GameSceneMenu | GameSceneLost;
        game.lastScore = game.pointsTxt->intAttribute1;
        updateHighScores(game.playerInfo.name, game.lastScore);

        for (int    i = game.preservedObjects,
                    l = game.objects.size();
                    i < l; ++i) {
            delete game.objects[i];
        }
        game.objects.erase(
            game.objects.begin() + game.preservedObjects, game.objects.end()
    );
        game.healthTxt->intAttribute1 = 100;
        game.healthTxt->doubleAttribute1 = 100.0f;
        game.pointsTxt->intAttribute1 = 0;
        game.upgrade1->intAttribute1 = 0;
        game.upgrade2->intAttribute1 = 0;
        game.isGamePaused = true;
    }
    if (game.scene & GameScenePlay) {
        FlipBook* fb = dynamic_cast<FlipBook*> (game.player->resource);
        fb->setFPS(std::max(game.player->vel.magnitude()* 10, 5.0f));
        game.healthBar->dim.x = (
                game.healthTxt->intAttribute1 /
                game.healthTxt->doubleAttribute1
                ) * 100.0f;
        std::stringstream ss;
        ss << game.healthTxt->intAttribute1;
        game.healthTxt->name = ss.str();
        ss.str("");
        ss << game.pointsTxt->intAttribute1;
        game.pointsTxt->name = ss.str();
        ss.str("");
        if ((game.timeTxt->doubleAttribute1 / 60) >= 1) {
            ss << (int) (game.timeTxt->doubleAttribute1 / 60);
            ss << " m ";
        }
        ss << std::fmod((game.timeTxt->doubleAttribute1), 60);
        ss << " s";
        game.timeTxt->name = ss.str();
        ss.str("");
        ss << game.highScoreTxt->intAttribute1;
        game.highScoreTxt->name = ss.str();
    }
    if (game.scene & GameSceneLost) {
        std::stringstream ss;
        ss << game.lastScore;
        game.pointsLast->name = ss.str();
    }
    if (game.scene & GameSceneUpgrades) {
        std::stringstream ss;
        ss.str("");
        ss << game.upgrade1->intAttribute1 << " (";
        ss << upgradeCurrentCost(game.upgrade1->intAttribute1) << ")";
        game.upgrade1->name = ss.str();

        ss.str("");
        ss << game.upgrade2->intAttribute1 << " (";
        ss << upgradeCurrentCost(game.upgrade2->intAttribute1) << ")";
        game.upgrade2->name = ss.str();

        ss.str("");
        ss << availablePoints();
        game.spendingScoreTxt->name = ss.str();

    }
    if (game.scene & GameSceneScore) {
        std::stringstream ss;
        ss << game.playerInfo.highScore;
        game.highScoreTxt->name = ss.str();
        ss.str("");
        ss << game.playerInfo.totalScore;
        game.totalScoreTxt->name = ss.str();
    }
}

/**
 * Listen to keyboard press events for the username menu when the game starts
 *
 */
void handleLoginInput(const XEvent& event)
{
    if (event.type == KeyPress) {
        char seq[32];
        KeySym keysym;
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        int len = XLookupString(
            const_cast<XKeyEvent*> (&event.xkey),
            seq, 25, &keysym, NULL
        );
        if (len > 0) {
            if (    key == XK_Return && game.loginTxt->name != "<Enter>" &&
                    game.loginTxt->name.size() >= 4) {
                game.playerInfo.name = game.loginTxt->name;
                game.scene &= ~GameSceneLogin;
                updateHighScores(game.playerInfo.name, 0);
            } else if ( key == XK_BackSpace &&
                        game.loginTxt->name != "<Enter>" &&
                        !game.loginTxt->name.empty()) {
                game.loginTxt->name =
                    game.loginTxt->name.substr(
                        0, game.loginTxt->name.size() - 1
                    );
                if (game.loginTxt->name.empty()) {
                    game.loginTxt->name = "<Enter>";
                }
            } else if (('0' <= seq[0] && seq[0] <= '9') ||
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
