
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <vector>
#include <vector>
#include <algorithm>
#include <ostream>
#include <map>
#include <set>
#include <climits>

#include "ds/core/World.h"
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
    typename ObjectMapType::iterator
        res = this->internal->objs.find(key), 
        end = this->internal->objs.end();;
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
    return this->internal->objs.erase(
                std::find_if(
                    this->internal->objs.begin(),
                    this->internal->objs.end(),
                    compareObject(obj)
                ))
            != this->internal->objs.end();
}

bool ds::core::World::remove(const ObjectKeyType& key)
{
    return this->internal->objs.erase(key) > 0;
}

void ds::core::World::addRenderer (render::Renderer* renderer)
{
    this->internal->renderers.push_back(renderer);
}

struct RenderableComparator {
    
    bool operator()(
        const ds::core::Object* lhs, 
        const ds::core::Object* rhs)
    {
        //get z order if available, or INT_MIN
        int lhsOrder = lhs->renderable ? 
                            lhs->renderable->getZOrder() : 
                            ds::render::ZOrder::min;
        //get z order if available, or INT_MIN
        int rhsOrder = rhs->renderable->getZOrder() ?
                            rhs->renderable->getZOrder() : 
                            ds::render::ZOrder::min;
        return lhsOrder < rhsOrder;
    }
};

struct ToSecondValue {
    ObjectMapType::mapped_type operator()(
        ObjectMapType::value_type& pair)
    {
        return pair.second;
    }
};

//Render the entire world
//First step is to copy all renderables into a set
//which sorts the renderables by z-order
//next, it loops through all the renderables
//  while looping through all the renderers
//      checks if a renderer supports a renderable
//          if it does, then call render and break out of loop
//  repeat until everything is rendered
void ds::core::World::render (render::RenderContext* ctx)
{
    ObjectMapType& renderablePairs = this->internal->objs;
    std::vector<ds::render::Renderer*>& renderers = this->internal->renderers;
    
    typedef std::multiset<
        typename ObjectMapType::mapped_type,
        RenderableComparator
    > ObjectSet;

    ObjectSet renderablesSet;
    //use transform using inserter and ToSecondValue transforming function 
    std::transform(
        renderablePairs.begin(), 
        renderablePairs.end(),
        std::inserter(
            renderablesSet,
            renderablesSet.begin()
        ),
        ToSecondValue()
    );

    for (typename ObjectSet::iterator
            it = renderablesSet.begin(), 
            end = renderablesSet.end(); 
            it != end; 
            ++it) {
        typename ObjectMapType::mapped_type obj = *it;
        if (obj->renderable) {
            for (std::vector<ds::render::Renderer*>::iterator 
                        rit = renderers.begin(), rend = renderers.end();
                        rit != rend;
                        ++rit) {
                ds::render::Renderer* r = *rit;                
                if(r->isRenderer(obj->renderable)) {
                    r->render(ctx, obj, obj->renderable);
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