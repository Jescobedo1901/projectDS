#include <vector>
#include "ds/impl/DSEngine.h"
#include "ds/core/Application.h"

#include "ds/impl/DSEventHandler.h"
#include "ds/impl/DSPhysicsHandler.h"
#include "ds/impl/DSRenderingHandler.h"
#include "ds/impl/DSAudioHandler.h"
#include "ds/core/World.h"



ds::impl::DSNewtonianPhysics::DSNewtonianPhysics (DSEngine* e) : eng (e)
{
}

void ds::impl::DSNewtonianPhysics::operator() (ds::core::fp_type delta)
{
    auto objs = this->eng->getWorld()->getObjects();
    for (auto
            it = objs.begin(),
            end = objs.end();
            it != end;
            ++it) {
        core::ObjectPtr obj = *it;

        core::Vec3 F = obj->cumForces();
        obj->forces.clear(); //clear applied forces

        // a = F / m
        core::Vec3 a = F / obj->mass;

        //Velocity  = acceleration * time
        obj->vel += delta * a;

        //Position = velocity * time
        // Adjust pixel to meter radio
        obj->pos += delta * obj->vel * impl::PIXEL_TO_METER;
//
//        std::cout << "Mass: " <<  obj->mass << std::endl;
//        std::cout << "F: " <<  F << std::endl;
//        std::cout << "Acc: " <<  obj->acc << std::endl;
//        std::cout << "Vel: " <<  obj->vel << std::endl;
//        std::cout << "Pos: " <<  obj->pos << std::endl;
    }

}

ds::impl::DSGravity::DSGravity (ds::core::Vec3 acc, ds::impl::DSEngine* e) : acceleration(acc), eng (e)
{
}

void ds::impl::DSGravity::operator() (ds::core::fp_type delta)
{
    auto objs = this->eng->getWorld()->getObjects();
    for (auto
            it = objs.begin(),
            end = objs.end();
            it != end;
            ++it) {                        
        (*it)->forces.emplace_back((*it)->mass * acceleration);
    }
}



ds::impl::DSEngine::DSEngine ()
    :   core::Engine (),
        eventHandler (this),
        physicsHandler (),
        renderingHandler (this),
        audioHandler (),
        app (nullptr),
        world (std::make_unique<core::World>())
{
    this->getPhysicsHandler()->addProcessor(std::make_shared<DSNewtonianPhysics>(this));
    this->getPhysicsHandler()->addProcessor(std::make_shared<DSGravity>(core::Vec3{0, -9.8, 0}, this));
}

ds::impl::DSEngine::~DSEngine ()
{
}

ds::impl::DSAudioHandler* ds::impl::DSEngine::getAudioHandler ()
{
    return &audioHandler;
}

ds::impl::DSEventHandler* ds::impl::DSEngine::getEventHandler ()
{
    return &eventHandler;
}

ds::impl::DSPhysicsHandler* ds::impl::DSEngine::getPhysicsHandler ()
{
    return &physicsHandler;
}

ds::impl::DSRenderingHandler* ds::impl::DSEngine::getRenderingHandler ()
{
    return &renderingHandler;
}

std::vector<ds::core::TaskHandler> ds::impl::DSEngine::getHandlers ()
{
    return {
        ds::core::TaskHandler(eventHandler),
        ds::core::TaskHandler(physicsHandler),
        ds::core::TaskHandler(renderingHandler),
        ds::core::TaskHandler(audioHandler)};
}

ds::core::World* ds::impl::DSEngine::getWorld ()
{
    return & * this->world;
}

ds::render::RenderContext* ds::impl::DSEngine::getRenderContext ()
{
    return this;
}

void ds::impl::DSEngine::attach (ds::core::Application* application)
{
    core::X11Application* attempt_cast = dynamic_cast<core::X11Application*> (application);
    if (attempt_cast) {
        this->app = attempt_cast;
    } else {
        throw core::EngineException("X11 support only");
    }
}

void ds::impl::DSEngine::detach (core::Application* appl)
{
    this->app = nullptr;
}

ds::core::X11Application* ds::impl::DSEngine::getApplication ()
{
    return this->app;
}

