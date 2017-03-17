
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <vector>

#include "ds/core/World.h"
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <ostream>

#include "ds/render/Rendering.h"

std::ostream& ds::core::operator<<(std::ostream& os, const ds::core::Vec3& v) {
    return os << '{' <<v.x << ", " << v.y << ", " << v.z << '}';
}

struct ds::core::World::Pimpl {
    std::unordered_map<
        ds::core::World::ObjectKeyType,
        ds::core::ObjectPtr
    > objs;
    std::vector<ds::render::RendererPtr> renderers;
    std::shared_timed_mutex objsMtx;
    std::shared_timed_mutex renderersMtx;
};

ds::core::World::World () :
internal (std::make_unique<ds::core::World::Pimpl>())
{
}

ds::core::World::~World ()
{
}

bool ds::core::World::add (const ObjectKeyType& key, ObjectPtr obj)
{
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    auto vec = this->internal->objs;
    auto end = vec.end();
    auto res = this->internal->objs.find(key);
    if (res == end) {
        this->internal->objs[key] = obj;
        return true;
    } else {
        return false;
    }
}

bool ds::core::World::remove (ObjectPtr obj)
{
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);

    return std::find_if(
        this->internal->objs.begin(),
        this->internal->objs.end(),
        [obj] (const auto& p) { return p.second == obj; }
    ) != this->internal->objs.end();
}

bool ds::core::World::remove(const ObjectKeyType& key)
{
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    return this->internal->objs.erase(key) > 0;
}

void ds::core::World::addRenderer (std::shared_ptr<render::Renderer> renderer)
{
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    this->internal->renderers.push_back(renderer);
}

//Render the entire world

void ds::core::World::render (render::RenderContext* ctx)
{
    //deadlock free multilock
    std::lock(this->internal->objsMtx, this->internal->renderersMtx);
    std::shared_lock <std::shared_timed_mutex>
            readObjects(this->internal->objsMtx, std::adopt_lock);
    std::shared_lock <std::shared_timed_mutex>
            readRenderers(this->internal->renderersMtx, std::adopt_lock);
    auto& vec = this->internal->objs;
    auto& renderers = this->internal->renderers;
    //Simple rendering logic
    //@TODO Optimize
    for (auto& e : vec) {
        if (e.second->renderable) {
            for (auto& r : renderers) {
                if(r->isRenderer(&*e.second->renderable)) {
                    r->render(ctx, &*e.second, &*e.second->renderable);
                    break;
                }
            }
        }
    }
}

std::vector<ds::core::ObjectPtr> ds::core::World::getObjects ()
{
    std::shared_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    std::vector<ObjectPtr> copy;
    copy.reserve(this->internal->objs.size());

    for (const auto &s : this->internal->objs)
        copy.push_back(s.second);

    return copy;
}

ds::core::ObjectPtr ds::core::World::getObject(const ObjectKeyType& key)
{
    std::shared_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    auto res =  this->internal->objs.find(key);
    if(res != this->internal->objs.end()) {
        return (*res).second;
    } else {
        return nullptr;
    }
}