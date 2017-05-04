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

bool isOffscreen(Object* obj)
{
    return obj->pos.x <= game.player->pos.x - game.xres;
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
    tex(NULL), texId(), texTransUsingFirstPixel(true),
    tolerance(tol)
{
    glEnable(GL_TEXTURE_2D);
    //If textureFile does not end with .ppm,
    //we must map it to the generated .ppm first
    std::string ext(".ppm");
    if (texFile.size() > ext.size() &&
            texFile.compare(texFile.size() - ext.size(), ext.size(), ext)
            != 0) {
        std::string mappedFile = texFile + ext;
        this->tex = ppm6GetImage(mappedFile.c_str());
    } else {
        this->tex = ppm6GetImage(texFile.c_str());
    }
    glGenTextures(1, &this->texId);
    int w = this->tex->width;
    int h = this->tex->height;
    glBindTexture(GL_TEXTURE_2D, this->texId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //TRANSPARENCY
    unsigned char *texAlphaData = buildAlphaData(
        this->tex,
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

TextureResource::~TextureResource() {
    delete this->tex;
}

GLuint TextureResource::getResourceId() {
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
    current(NULL), book() {
    //find matching textures file with glob
    glob_t results;
    if(glob(pathWildcard.c_str(), 0, NULL, & results) != 0) {
        initFailure(("Failed globbing textures for: " + pathWildcard).c_str());
    } else {
        for (size_t i = 0; i < results.gl_pathc; i++) {
            this->book.push_back(new TextureResource(results.gl_pathv[i], tolerance));
        }
        globfree(& results);
        if(this->book.size() > 0) {
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

/**
 * Lookup resources by mapped name to the actual resource name
 * @param obj
 * @param resourceName
 */
void mapResource(Object* obj, const char* resourceName)
{
    ResourceMap::iterator it = game.resourceMap.find(resourceName);
    if(it != game.resourceMap.end()) {
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
            end =game.resourceMap.end();
            it != end;
            ++it) {
        FlipBook* fp = dynamic_cast<FlipBook*>((*it).second);
        if(fp) {
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
        int tol )
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
        *(ptr + 3) = !(std::abs(a - ta) <= tol && std::abs(b - tb) <= tol && std::abs(c - tc) <= tol);
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
    for(p = servinfo; p != NULL; p = p->ai_next) {
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
    ss      << "GET " << page << username
            << "&score=" << latestHigh
            << " HTTP/1.0\r\nHost: " << host <<
            "\r\nUser-Agent: " << useragent << "\r\n\r\n";

    std::string request = ss.str();
    const char * requestGet = request.c_str();

    int sent = 0;
    int getReqSize = strlen(requestGet);

    while(sent < getReqSize) {
        int sentBytes = write(sockfd, requestGet, strlen(requestGet));
        if(sentBytes == -1) {
            herror("Failed sending data");
            return -1;
        }
        sent += getReqSize;
    }

    std::string result;
    char buffer[1024 * 32] = { 0 };
    int bytesRead = -1;

    do {
        bytesRead = recv(sockfd, buffer, sizeof(buffer)-1, 0);
        if(bytesRead > 0) {
            result += buffer;
            bzero(buffer, sizeof(buffer));
        }
    }while (bytesRead > 0);

    unsigned int subidx = result.find("\r\n\r\n") + 4;

    if(subidx < result.size()) {
        result = result.substr(subidx);
        ss.str("");
        ss << result;
        std::string user;
        int total, max;

        std::vector<Score> scores;

        while(ss >> user >> total >> max) {
            scores.push_back(Score(user, total, max));
        }

        if(!scores.empty()) {
            Score last = *(scores.begin() + scores.size()-1);
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
        for(int i = game.scoreObjects.size()-1; i >= 0; --i) {
            if(scoresIt != scores.end()) {
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
    if(obj->objectType == ObjectTypeEnemy) {
        if(obj->doubleAttribute1 == 0) {
            //If double attribute is not set, see the motion pattern here
            obj->doubleAttribute1 = (float)rand() / (float)RAND_MAX;
        }

        float ymin = getOceanFloorUpperBound(obj->pos.x);
        float ymax = getOceanUpperBound(obj->pos.x);

        float time = obj->pos.x;
        float phase = 0.0f;
        float freq = 0.025f * stepDuration *  obj->doubleAttribute1;
        float amplitude = ((ymax-ymin) / 2.0f);
        float shift = ymax/2.0f;

        obj->pos.y = amplitude * std::sin(2 * M_PI * freq * time + phase) + shift;
    }
}