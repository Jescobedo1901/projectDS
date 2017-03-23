//Omar Thor
//Group 4
//DeepSea Survival Game

#include <X11/Xlib.h>
#include <cmath>
#include <sstream>
#include "ds/impl/DSEngine.h"
#include "ds/core/World.h"
#include "fonts.h"

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
            core::fp_type 
                    rad = obj->avgRadius,
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


struct TextRenderer : render::Renderer {

     TextRenderer() : rect()
    {
    }

    bool isRenderer (render::Renderable* render) {
        return dynamic_cast<render::Text*>(render);
    }

    void render (render::RenderContext* ctx, core::Object* obj, render::Renderable* renderable)
    {
        render::Text* txt = dynamic_cast<render::Text*>(renderable);
        rect.bot = obj->pos.y;
	rect.left = obj->pos.x;
	rect.center = 0;
        int cref = txt->color.toInt();
        switch(txt->style) {
            case render::plain6:
                ggprint06(&rect, 0, cref, txt->text.c_str());
            break;
            case render::plain7:
                ggprint07(&rect, 0, cref, txt->text.c_str());
                break;
            case render::plain8:
                ggprint08(&rect, 0, cref, txt->text.c_str());
                break;
            case render::bold8:
                ggprint8b(&rect, 0, cref, txt->text.c_str());
                break;
            case render::plain10:
                ggprint10(&rect, 0, cref, txt->text.c_str());
                break;
            case render::plain12:
                ggprint12(&rect, 0, cref, txt->text.c_str());
                break;
            case render::plain13:
                ggprint13(&rect, 30,   cref, txt->text.c_str());
                break;
            case render::plain16:
                ggprint16(&rect, 30,   cref, txt->text.c_str());
                break;
            case render::plain40:
                ggprint40(&rect, 0,    cref, txt->text.c_str());
            break;
            default:
                ggprint06(&rect, 0, cref, txt->text.c_str());
                break;
        }
    }
private:
    Rect rect;
};

//Physics section
enum Direction {
    DirLeft = 1, DirRight = 2, DirUp = 4, DirDown = 8, DirNone = 16
};

struct PlayerMovementTracker : PhysicsProcessor {
    
    PlayerMovementTracker(DSEngine* e) : dirMask(DirNone), eng(e), idx(0)
    {
    }

    void operator()(core::fp_type delta) {
        if(eng->getWorld()->getObjects().begin() != eng->getWorld()->getObjects().end()) {
            core::Object* obj = eng->getWorld()->getObject("player/token");
            if(obj) {
                core::fp_type thrust = 200 * obj->mass;
                if(this->dirMask & DirUp)
                    obj->forces.push_back(core::Vec3(0, thrust, 0));
                if(this->dirMask & DirDown)
                    obj->forces.push_back(core::Vec3(0, -thrust, 0));
                if(this->dirMask & DirRight)
                    obj->forces.push_back(core::Vec3(thrust, 0, 0));
                if(this->dirMask & DirLeft)
                    obj->forces.push_back(core::Vec3(-thrust, 0, 0));
                core::Object* healthBar = eng->getWorld()->getObject("player/health");
                std::stringstream ss;
                ss << "Health level: " << (idx++);
                dynamic_cast<render::Text*>(healthBar->renderable)->text = ss.str();
            }
        }
    }

    int dirMask;    
private:

    DSEngine* eng;
    int idx;
};

//Event section

struct PlayerMovement : EventProcessor {

    PlayerMovement(
        PlayerMovementTracker* tracker,
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
    PlayerMovementTracker* tracker;
    ds::impl::DSEngine* eng;
};

struct ExitEventProcessor : EventProcessor {
    ExitEventProcessor(DSEngine* e)
        :   eng(e),
            wmDeleteMessage()
    {
        //util::XLockDisplayGuard(this->app->display);
        this->wmDeleteMessage = XInternAtom(
            eng->getApplication()->display, "WM_DELETE_WINDOW", False
        );
        XSetWMProtocols(
            eng->getApplication()->display, 
            eng->getApplication()->win, &wmDeleteMessage, 
            1
        );
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
    eng->getWorld()->addRenderer(new TextRenderer());
    eng->getWorld()->addRenderer(new SphereRenderer());

    //Adding physics effect
    PlayerMovementTracker* playerTracker = 
        eng->getPhysicsHandler()->addProcessor(
            new PlayerMovementTracker(eng)
        );

    //Adding event handler for player movement
    eng->getEventHandler()->addProcessor(
        new PlayerMovement(playerTracker, eng)
    );

    //Adding event handler for click X event
    eng->getEventHandler()->addProcessor(
        new ExitEventProcessor(eng)
    );

    //Demo code for a test ball
    core::Object* testBall = new core::Object();

    testBall->name = "Test object";
    testBall->avgRadius = 10;
    testBall->pos.y = 400;
    testBall->pos.x = 400;
    testBall->renderable = new render::Sphere();
    testBall->mass = 100;

    core::Object* healthBar = new core::Object();

    render::Text* healthText = new render::Text();

    healthBar->pos.x = 5;
    healthBar->pos.y = 580;
    healthText->style = render::plain12;
    healthText->color = render::Color(255, 255, 255, 255);
    healthBar->renderable =  healthText;
    healthBar->getAttrs().set<int>("level", 100);
   
    eng->getWorld()->add("player/token", testBall);
    eng->getWorld()->add("player/health", healthBar);
}


}}