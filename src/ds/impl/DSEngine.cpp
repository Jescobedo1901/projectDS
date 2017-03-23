#include <vector>
#include <unistd.h>
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
    std::vector<ds::core::Object*> objs = this->eng->getWorld()->getObjects();
    for (std::vector<ds::core::Object*>::iterator
            it = objs.begin(),
            end = objs.end();
            it != end;
            ++it) {
        core::Object* obj = *it;

        core::Vec3 force = obj->cumForces();
        obj->forces.clear(); //clear applied forces

        //Apply only to objects with mass
        if(obj->mass) {
            // a = F / m
            core::Vec3 a = force / obj->mass;

            //Velocity  = acceleration * time
            obj->vel += delta * a;

            //Position = velocity * time
            // Adjust pixel to meter radio
            obj->pos += delta * obj->vel * impl::PIXEL_TO_METER;
        }
    }

}

ds::impl::DSGravity::DSGravity (ds::core::Vec3 acc, ds::impl::DSEngine* e) : acceleration(acc), eng (e)
{
}

void ds::impl::DSGravity::operator() (ds::core::fp_type delta)
{
    std::vector<ds::core::Object*> objs = this->eng->getWorld()->getObjects();
    for (std::vector<ds::core::Object*>::iterator
            it = objs.begin(),
            end = objs.end();
            it != end;
            ++it) {
        (*it)->forces.push_back((*it)->mass * acceleration);
    }
}



ds::impl::DSEngine::DSEngine ()
    :   core::Engine (),
        eventHandler (this),
        physicsHandler (),
        renderingHandler (this),
        audioHandler (),
        app (NULL),
        world (),
        done(false)
{
    this->getPhysicsHandler()->addProcessor(new DSNewtonianPhysics(this));
    this->getPhysicsHandler()->addProcessor(new DSGravity(core::Vec3(0, -9.8, 0), this));
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

ds::core::World* ds::impl::DSEngine::getWorld ()
{
    return &this->world;
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
        core::init_failure("X11 support only");
    }
}

inline double diff(timespec& start, timespec& end) {
    return                  \
        (start.tv_sec - end.tv_sec )
        +
        (double) (start.tv_nsec - end.tv_nsec) * 1e-9;
}
#define CALC_DIFF(start, end)                   \

#define ADD_FP(time, fp)                        \
        (time).tv_sec += (time_t) fp;           \
        (time).tv_nsec += (time_t)((fp - (double) (time).tv_sec)* 1.0e9);

void ds::impl::DSEngine::run()
{
    const double tickSeconds = 1.0 / 60.0;

    //Used to measure the real time passed, for delta
    struct timespec real;
    clock_gettime(CLOCK_REALTIME, &real);

    //Used to measure the timer expiration
    struct timespec prev, curr;
    clock_gettime(CLOCK_REALTIME, &prev);

    //temporary calculation results
    struct timespec now;

    while (!this->done) {

        //render frame
        this->renderingHandler.apply();

        //tick game state
        clock_gettime(CLOCK_REALTIME, &now);
        double dt = diff(now, prev);
        while (dt >= tickSeconds) {
            dt -= tickSeconds;
            this->physicsHandler.apply(tickSeconds);
            this->eventHandler.apply();
            this->audioHandler.apply();
        }

        prev = now;
    }
}

void ds::impl::DSEngine::detach (core::Application* appl)
{
    this->app = NULL;
}

void ds::impl::DSEngine::init()
{
    this->getRenderingHandler()->init();
}
ds::core::X11Application* ds::impl::DSEngine::getApplication ()
{
    return this->app;
}

