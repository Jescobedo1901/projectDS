//Omar Thor
//Group 4
//DeepSea Survival Game

#include <X11/Xlib.h>
#include <cmath>
#include <memory>
#include "ds/impl/DSEngine.h"
#include "ds/core/World.h"

namespace ds { namespace impl {

//Rendering section
struct SphereRenderer : public render::Renderer {
    SphereRenderer()
    {
    }

    bool isRenderer (render::Renderable* render) {
        return dynamic_cast<render::Sphere*>(render);
    }

    void render (render::RenderContext* ctx, core::Object* obj, render::Renderable* renderable)
    {
        glColor4f(1.0f,1.0f,1.0f,1.0f);
        glBegin(GL_TRIANGLE_FAN);
            auto    rad = obj->avgRadius,
                    x = obj->pos.x,
                    y = obj->pos.y;
            for (int i = 0; i < 360; ++i)
            {
               float degInRad = i*M_PI/180;
               glVertex2f(
                    std::cos(degInRad)*rad + x,
                    std::sin(degInRad)*rad + y
                );
            }
        glEnd();
    }

};

//Physics section
enum Direction {
    DirLeft = 1, DirRight = 2, DirUp = 4, DirDown = 8, DirNone = 16
};

struct PlayerMovementTracker : PhysicsProcessor {
    PlayerMovementTracker(DSEngine* e) : dirMask(DirNone), eng(e)
    {
    }

    void operator()(core::fp_type delta) {        
        if(eng->getWorld()->getObjects().begin() != eng->getWorld()->getObjects().end()) {
            core::ObjectPtr obj = *eng->getWorld()->getObjects().begin();
            if(obj) {        
                auto thrust = 20 * obj->mass;
                if(this->dirMask & DirUp)
                    obj->forces.push_back({0, thrust, 0});
                if(this->dirMask & DirDown)
                    obj->forces.push_back({0, -thrust, 0});
                if(this->dirMask & DirRight)
                    obj->forces.push_back({thrust, 0, 0});
                if(this->dirMask & DirLeft)
                    obj->forces.push_back({-thrust, 0, 0});
            }
        }
    }

    int dirMask;
private:

    DSEngine* eng;
};

//Event section

struct PlayerMovement : EventProcessor {

    PlayerMovement(
        std::shared_ptr<PlayerMovementTracker> tracker,
        impl::DSEngine* e
    ) : tracker(tracker), eng(e)
    {
    }

    void operator()(const XEvent& event) {
        if(event.type == KeyPress) {
            int key = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
            if(key == XK_Up) {
                tracker->dirMask |= DirUp;
            }
            if(key == XK_Down) {
                tracker->dirMask |= DirDown;
            }
            if(key == XK_Left) {
                tracker->dirMask |= DirLeft;
            }
            if(key == XK_Right) {
                tracker->dirMask |= DirRight;
            }
        } else if(event.type == KeyRelease) {
            int key = XLookupKeysym(const_cast<XKeyEvent*>(&event.xkey), 0);
            if(key == XK_Up) {
                tracker->dirMask &= ~DirUp;
            }
            if(key == XK_Down) {
                tracker->dirMask &= ~DirDown;
            }
            if(key == XK_Left) {
                tracker->dirMask &= ~DirLeft;
            }
            if(key == XK_Right) {
                tracker->dirMask &= ~DirRight;
            }            
        }
    }

private:
    std::shared_ptr<PlayerMovementTracker> tracker;
    ds::impl::DSEngine* eng;
};

struct ExitEventProcessor : EventProcessor {
    ExitEventProcessor(DSEngine* e)
        :   eng(e),
            wmDeleteMessage()
    {
        auto app = eng->getApplication();
        //util::XLockDisplayGuard(this->app->display);
        this->wmDeleteMessage = XInternAtom(
            app->display, "WM_DELETE_WINDOW", False
        );
        XSetWMProtocols(app->display, app->win, &wmDeleteMessage, 1);
    }

    void operator()(const XEvent& event) {
        //Handled exit gracefully
        if(event.type == ClientMessage) {
            if(static_cast<unsigned long>(event.xclient.data.l[0]) == wmDeleteMessage) {
                eng->getApplication()->exit();
            }
        }
    }
private:
    DSEngine* eng;
    Atom wmDeleteMessage;
};


/**
 * Initialization hook for Omar's code
 */
void initOmarO(DSEngine* eng) {
    verify(eng);

    //Adding renderers
    eng->getWorld()->addRenderer(std::make_shared<SphereRenderer>());
    
    //Adding physics effect
    auto playerTracker = eng->getPhysicsHandler()->addProcessor(
        std::make_shared<PlayerMovementTracker>(eng)
    );
    //Adding event handler for player movement
    eng->getEventHandler()->addProcessor(
        std::make_shared<PlayerMovement>(playerTracker, eng)
    );
    //Adding event handler for click X event
    eng->getEventHandler()->addProcessor(
        std::make_shared<ExitEventProcessor>(eng)
    );
    
    
    //Demo code for a test ball
    core::ObjectPtr testBall = std::make_shared<core::Object>();

    testBall->name = "Test object";
    testBall->avgRadius = 10;
    testBall->pos.y = 300;
    testBall->pos.x = 400;
    testBall->renderable = std::make_shared<render::Sphere>();
    testBall->mass = 100;
    eng->getWorld()->add(testBall);

}


}}