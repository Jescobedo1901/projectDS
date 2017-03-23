
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <vector>

#include "ds/core/World.h"
#include <vector>
#include <algorithm>
#include <ostream>
#include <map>

#include "ds/render/Rendering.h"

std::ostream& ds::core::operator<<(std::ostream& os, const ds::core::Vec3& v) {
    return os << '{' <<v.x << ", " << v.y << ", " << v.z << '}';
}
typedef std::map<
            ds::core::World::ObjectKeyType,
            ds::core::Object*
        > ObjectMapType;

struct ds::core::World::Pimpl {
    ObjectMapType objs;
    std::vector<ds::render::Renderer*> renderers;
};

ds::core::World::World ()
    : internal (new ds::core::World::Pimpl())
{
}

ds::core::World::~World ()
{
    //cleanup owned stuff
    for(ObjectMapType::iterator 
            it = this->internal->objs.begin(),
            end = this->internal->objs.end();
            it != end;
            ++it) {
        delete (*it).second;
    }    
    for(std::vector<ds::render::Renderer*>::iterator 
            it = this->internal->renderers.begin(),
            end = this->internal->renderers.end();
            it != end;
            ++it) {
        delete *it;
    }
    delete this->internal;
}

bool ds::core::World::add (const ObjectKeyType& key, Object* obj)
{
    ObjectMapType& vec = this->internal->objs;
    typename ObjectMapType::iterator
        res = this->internal->objs.find(key), 
        end = vec.end();;
    if (res == end) {
        this->internal->objs[key] = obj;
        return true;
    } else {
        return false;
    }
}
struct compareObject
{
    compareObject(ds::core::Object* obj)
        : obj(obj)
    {
    }

    bool operator()(const ObjectMapType::value_type& p)
    {
        return p.second == obj;
    }
    
    ds::core::Object* obj;
};
bool ds::core::World::remove (Object* obj)
{
    return std::find_if(
        this->internal->objs.begin(),
        this->internal->objs.end(),
        compareObject(obj)
    ) != this->internal->objs.end();
}

bool ds::core::World::remove(const ObjectKeyType& key)
{
    return this->internal->objs.erase(key) > 0;
}

void ds::core::World::addRenderer (render::Renderer* renderer)
{
    this->internal->renderers.push_back(renderer);
}

//Render the entire world

void ds::core::World::render (render::RenderContext* ctx)
{
    ObjectMapType& vec = this->internal->objs;
    std::vector<ds::render::Renderer*>& renderers = this->internal->renderers;
    //Simple rendering logic
    //@TODO Optimize
//    for (auto& e : vec) {
//        if (e.second->renderable) {
//            for (auto& r : renderers) {
//                if(r->isRenderer(&*e.second->renderable)) {
//                    r->render(ctx, &*e.second, &*e.second->renderable);
//                    break;
//                }
//            }
//        }
//    }
    
    for (typename ObjectMapType::iterator 
            it = vec.begin(), 
            end = vec.end(); 
            it != end; 
            ++it) {
        typename ObjectMapType::value_type e = *it;
        if (e.second->renderable) {                    
            for (std::vector<ds::render::Renderer*>::iterator 
                        rit = renderers.begin(), rend = renderers.end();
                        rit != rend;
                        ++rit) {
                ds::render::Renderer* r = *rit;                
                if(r->isRenderer(e.second->renderable)) {
                    r->render(ctx, e.second, e.second->renderable);
                    break;
                }
            }
        }
    }    
}

std::vector<ds::core::Object*> ds::core::World::getObjects ()
{
    std::vector<Object*> copy;
    copy.reserve(this->internal->objs.size());

    for (typename ObjectMapType::iterator 
            it = this->internal->objs.begin(), 
            end = this->internal->objs.end(); 
            it != end; 
            ++it) {
        const ObjectMapType::value_type& s = *it;
        copy.push_back(s.second);
    }
    
    return copy;
}

ds::core::Object* ds::core::World::getObject(const ObjectKeyType& key)
{
    typename ObjectMapType::iterator res =  this->internal->objs.find(key);
    if(res != this->internal->objs.end()) {
        return (*res).second;
    } else {
        return NULL;
    }
}