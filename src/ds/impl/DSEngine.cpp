#include <vector>
#include "ds/impl/DSEngine.h"
#include "ds/core/Application.h"

#include "ds/impl/DSEventHandler.h"
#include "ds/impl/DSPhysicsHandler.h"
#include "ds/impl/DSRenderingHandler.h"
#include "ds/impl/DSAudioHandler.h"
#include "ds/core/World.h"


ds::impl::DSEngine::DSEngine()
    :   core::Engine(),
        eventHandler(this),
        physicsHandler(),
        renderingHandler(this),
        audioHandler(),
        app(nullptr),
        world(std::make_unique<core::World>()) {}

ds::impl::DSEngine::~DSEngine() {
}

ds::impl::DSAudioHandler* ds::impl::DSEngine::getAudioHandler() {
    return &audioHandler;
}

ds::impl::DSEventHandler* ds::impl::DSEngine::getEventHandler() {
    return &eventHandler;
}

ds::impl::DSPhysicsHandler* ds::impl::DSEngine::getPhysicsHandler() {
    return &physicsHandler;
}

ds::impl::DSRenderingHandler* ds::impl::DSEngine::getRenderingHandler() {
    return &renderingHandler;
}

std::vector<ds::core::TaskHandler> ds::impl::DSEngine::getHandlers() {
    return {
        ds::core::TaskHandler(eventHandler),
        ds::core::TaskHandler(physicsHandler),
        ds::core::TaskHandler(renderingHandler),
        ds::core::TaskHandler(audioHandler)
    };
}

ds::core::World* ds::impl::DSEngine::getWorld() {
    return &*this->world;
}

ds::render::RenderContext* ds::impl::DSEngine::getRenderContext() {
    return this;
}

void ds::impl::DSEngine::attach(ds::core::Application* application) {
    core::X11Application* attempt_cast = dynamic_cast<core::X11Application*>(application);
    if(attempt_cast) {
        this->app = attempt_cast;
    } else {        
        throw core::EngineException("X11 support only");
    }
}

void ds::impl::DSEngine::detach(core::Application* appl) {
    this->app = nullptr;
}

ds::core::X11Application* ds::impl::DSEngine::getApplication() {
    return this->app;
}