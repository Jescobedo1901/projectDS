
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include <vector>

#include "ds/core/World.h"
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include "ds/render/Rendering.h"


struct ds::core::World::Pimpl {
    std::vector<ds::core::ObjectPtr> objs;
    std::vector<ds::render::Renderer*> renderers;
    std::shared_timed_mutex objsMtx;
    std::shared_timed_mutex renderersMtx;
};

ds::core::World::World() :
    internal(std::make_unique<ds::core::World::Pimpl>()) {
}

ds::core::World::~World() {}

bool ds::core::World::add(ObjectPtr obj) {
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    auto vec = this->internal->objs;
    auto end = vec.end();
    if(std::find(vec.begin(), end, obj) == end) {
        this->internal->objs.emplace_back(obj);
        return true;
    } else {
        return false;
    }
}
bool ds::core::World::remove(ObjectPtr obj) {
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    auto vec = this->internal->objs;
    auto end = vec.end();
    return vec.erase(std::remove(vec.begin(), end, obj), end) != end;    
}

void ds::core::World::addRenderer (render::Renderer* renderer) {
    std::unique_lock<std::shared_timed_mutex> lock(this->internal->objsMtx);
    this->internal->renderers.push_back(renderer);
}

//Render the entire world
void ds::core::World::render(render::RenderContext* ctx) {
    std::shared_lock <std::shared_timed_mutex> readObjects(this->internal->objsMtx);
    std::shared_lock <std::shared_timed_mutex> readRenderers(this->internal->renderersMtx);
    auto vec = this->internal->objs;
    auto renderers = this->internal->renderers;
    //Simple rendering logic
    //@TODO Optimize
    for(auto& e : vec) {
        if(e->renderable) {
            for(auto r: renderers) {
                if(e->renderable->isRenderer(r)) {
                    r->render(ctx, &*e, &*e->renderable);
                }
            }
        }
    }
}