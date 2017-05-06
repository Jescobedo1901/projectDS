//Omar Thor
//Group 4
//DeepSea Survival Game

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <iomanip>

#include "game.h"

void decompressedResourcesCleanup()
{
    DIR *d;
    struct dirent *dir;
    d = opendir("./images");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) {
                std::string ext(".ppm"), texFile(dir->d_name);
                //If textureFile does not end with .ppm, we must convert
                //it to .ppm first
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

void decompressResources()
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
        if (game.scene & GameSceneHUD) {
            renderObjects(GameSceneHUD);
        }
    }
    if (game.scene & ~(GameScenePlay | GameSceneHUD)) {
        renderMap();
        renderObjects(game.scene);
    }
    //audioLoop();
    glXSwapBuffers(game.display, game.win);
}

/**
 * Render the map onto the screen.
 * 
 * To keep things simple, the map is rendered inplace of the game's translation
 * This means that we only need to draw the map on the screen where the player
 * is located
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

//The following functions define the parameters of the map rendering

float getSkyUpperBound(int x)
{
    return game.yres;
}

float getSkyLowerBound(int x)
{
    return (.75 * game.yres);
}

float getOceanUpperBound(int x)
{
    return (.8 * game.yres) + 10 * 
            std::sin((x + game.mapBoundsIteration * 0.25) / 25.0);
}

float getOceanFloorUpperBound(int x)
{
    return 100 + 30 * std::sin(x / 100.0);
}

float getOceanFloorLowerBound(int x)
{
    return 0;
}

//This function is called every step

void stepMapBoundsIteration()
{

    //Step mapBoundsIteration
    //used to move waves forward in time
    ++game.mapBoundsIteration;
}

void stepPhysics(float stepDuration)
{
    /**
     * No physical effects are apply
     * unless the game scene loaded is play
     */
    if (game.scene & GameScenePlay) {

        stepMapBoundsIteration();
        applySpawnRate(stepDuration);

        stepFlipBooks(stepDuration);

        for (std::vector<Object*>::iterator 
            it = game.objects.begin(), 
            end = game.objects.end();
            it != end; ++it) {
            Object* obj = *it;
            if (applyObjectLifetimePolicies(obj)) {
                continue;
            }
            //Apply to all except object type enemy
            switch (obj->objectType) {
            case ObjectTypeEnemy:
                if (obj->name == "enemy3") {
                    applyGravity(obj);
                    break;
                } else if (obj->name == "ship" ||
                        obj->name == "ship-wreck") {
                    goto handleShip;
                }
            case ObjectTypeFriendly:
            case ObjectTypeNeutral:
                applyNonPlayerMotion(obj, stepDuration);
                break; //Just break, don't apply
            handleShip:
            default:
                applyGravity(obj);
                applyStokesApprox(obj);
                applyBuoyancyApprox(obj);
                break;
            }
            applyNewtonianPhysics(obj, stepDuration);
            switch (obj->objectType) {
            case ObjectTypePlayer:
                applyPlayerMovement(obj);
                applyPlayerDirChange(obj);
                applyPlayerOceanBurstMovement(obj, stepDuration);
                break;
            default:
                break;
            }
            applyRotationalHandling(obj, stepDuration);
            applyObjectBoundaryCollision(obj);
        }
        //Broad collision handling separately
        checkObjectCollisions();
    }
}

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

void applyStokesApprox(Object* obj)
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

void applyGravity(Object* obj)
{
    //Apply gravity on objects with mass
    if (obj->mass > 0) {
        obj->forces.push_back(obj->mass * Acceleration(0, -9.8, 0));
    }
}

/**
 * Applies resistance as a product of time
 * http://hyperphysics.phy-astr.gsu.edu/hbase/lindrg.html#c2
 * https://en.wikipedia.org/wiki/Drag_(physics)
 * https://en.wikipedia.org/wiki/Stokes'_law
 */
void applyBuoyancyApprox(Object* obj)
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

void applyPlayerOceanBurstMovement(Object* player, float stepDuration)
{
    if (    player->pos.y > getOceanUpperBound(player->pos.x) && 
            player->vel.y > 8.0f) {
        player->vel.y = 8.0f;
    }
}

bool isOffscreen(Object* obj)
{
    return obj->pos.x <= game.player->pos.x - game.xres * 0.5;
}

bool applyObjectLifetimePolicies(Object* obj)
{
    switch (obj->objectType) {
    case ObjectTypeFriendly:
    case ObjectTypeNeutral:
    case ObjectTypeEnemy:
        if (isOffscreen(obj)) {
            game.objects.erase(
                    std::remove(game.objects.begin(), game.objects.end(), obj)
                    );
            delete obj;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

Resource::Resource(std::string p)
: path(p)
{
}

Resource::~Resource()
{
}

TextureResource::TextureResource(std::string texFile, int tol)
: Resource(texFile),
texId(), texTransUsingFirstPixel(true),
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
    unsigned char *texAlphaData = buildAlphaData(
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

TextureResource::~TextureResource()
{
}

GLuint TextureResource::getResourceId()
{
    return this->texId;
}

/**
 * Maps multiple textures into a flip book
 * @param pathWildcard
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
 * This increments the frame forward synchronized
 * with the physical movement during play (GameScenePlay)
 * @param stepDuration
 */
void FlipBook::step(float stepDuration)
{
    this->duration += stepDuration;
    int frame = (int(this->duration * this->fps)) % this->book.size();
    this->current = this->book[frame];
}

GLuint FlipBook::getResourceId()
{
    return this->current->getResourceId();
}

void FlipBook::setFPS(float newFPS)
{
    this->duration = this->duration * this->fps / newFPS;
    this->fps = newFPS;
}

/**
 * Lookup resources by mapped name to the actual resource name
 * @param obj
 * @param resourceName
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

//ALPHA DATA FUNCTION

/**
 * Builds transparency texture using non-transparent
 * image data
 *
 * @param img
 * @param firstPixel If true, it uses the first pixel to determine the
 * color that is transparent
 * @param tol the tolerance of that pixel
 * @return
 */
unsigned char *buildAlphaData(
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
        if (obj->slowRotate) {
            obj->rotation += target * stepDuration * obj->rotationRate;
        } else {
            obj->rotation = target;
        }
    } else if (obj->rotationTarget != obj->rotation && obj->slowRotate) {
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
