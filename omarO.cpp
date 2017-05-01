//Omar Thor
//Group 4
//DeepSea Survival Game

#include "game.h"

void handlePlayerMovement(const XEvent& event)
{
    if (event.type == KeyPress) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up) {
            game.playerMovementDirectionMask |= DirUp;
        }
        if (key == XK_Down) {
            game.playerMovementDirectionMask |= DirDown;
        }
        if (key == XK_Left) {
            game.playerMovementDirectionMask |= DirLeft;
        }
        if (key == XK_Right) {
            game.playerMovementDirectionMask |= DirRight;
        }
    } else if (event.type == KeyRelease) {
        int key = XLookupKeysym(const_cast<XKeyEvent*> (&event.xkey), 0);
        if (key == XK_Up) {
            game.playerMovementDirectionMask &= ~DirUp;
        }
        if (key == XK_Down) {
            game.playerMovementDirectionMask &= ~DirDown;
        }
        if (key == XK_Left) {
            game.playerMovementDirectionMask &= ~DirLeft;
        }
        if (key == XK_Right) {
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
