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

void applyObjectLifetimePolicies(Object* obj)
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
        }
        break;
    default:
        break;
    }

}

Resource::Resource(std::string p)
: path(p)
{
}

Resource::~Resource()
{
}

TextureResource::TextureResource(std::string texFile)
    : Resource(texFile),
    tex(NULL), texId(), texTransUsingFirstPixel(true)
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
            this->texTransUsingFirstPixel
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
FlipBook::FlipBook(std::string pathWildcard, float framesPerSeconds)
    : Resource(pathWildcard), duration(), fps(framesPerSeconds),
    current(NULL), book() {
    //find matching textures file with glob
    glob_t results;
    if(glob(pathWildcard.c_str(), 0, NULL, & results) != 0) {
        initFailure(("Failed globbing textures for: " + pathWildcard).c_str());
    } else {
        for (size_t i = 0; i < results.gl_pathc; i++) {
            this->book.push_back(new TextureResource(results.gl_pathv[i]));
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