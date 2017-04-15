//Omar Thor
//Group 4
//DeepSea Survival Game

#include <X11/Xlib.h>
#include <cmath>
#include <sstream>
#include <set>
#include <utility>
#include <algorithm>
#include <climits>
#include <complex>

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

    void render (
        render::RenderContext* ctx, 
        core::Object* obj, 
        render::Renderable* renderable
    )
    {
        render::Sphere* sph = dynamic_cast<render::Sphere*>(renderable);
        int col = sph->color.toInt();        
        glBegin(GL_TRIANGLE_FAN);
        glColor4iv(&col);
            core::fp_type
                    rad = obj->avgRadius * PIXEL_TO_METER,
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

    void render (
        render::RenderContext* ctx,
        core::Object* obj,
        render::Renderable* renderable
    )
    {
        render::Text* txt = dynamic_cast<render::Text*>(renderable);
        rect.bot = obj->pos.y;
	rect.left = obj->pos.x;
	rect.center = 0;
        int cref = txt->color.toRGBInt();
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


struct Map : render::Renderable {

    Map() : Renderable(render::ZOrder::background), number(1)
    {
    }

    int number;
};

//Very simple unoptimized map renderer
//@TODO optimize
struct MapRenderer : render::Renderer {
    int iteration = 0;
    MapRenderer(MapBoundsGenerator* mapBG) : mbg(mapBG)
    {
    }

    bool isRenderer(render::Renderable* render)
    {
        return dynamic_cast<Map*>(render);
    }

    void render(render::RenderContext* ctx, core::Object* obj, render::Renderable* renderable)
    {
        for(int x = 0; x < 800; ++x) {
            glBegin(GL_LINES);

                glColor3ub(239, 245, 250);
                glVertex2d(x, mbg->getSkyUpperBound(x));
                glColor3ub(101, 188, 255);
                glVertex2f(x, mbg->getSkyLowerBound(x));

                glColor3ub(43, 175, 255);
                glVertex2d(x, mbg->getOceanUpperBound(x));
                glColor3ub(0, 75, 125);
                glVertex2f(x, mbg->getOceanFloorLowerBound(x));

                glColor3ub(221, 207, 18);
                glVertex2d(x, mbg->getOceanFloorUpperBound(x));
                glColor3ub(247, 223, 48);
                glVertex2f(x, mbg->getOceanFloorLowerBound(x));

            glEnd();
        }
    }

    MapBoundsGenerator* mbg;
};


//Physics section
enum Direction {
    DirLeft = 1, DirRight = 2, DirUp = 4, DirDown = 8, DirNone = 16
};

struct PlayerMovementTracker : PhysicsProcessor {

    int i = 0;
    PlayerMovementTracker(DSEngine* e) : dirMask(DirNone), eng(e), idx(0)
    {
    }

    void operator()(core::fp_type delta) {
        core::Object* obj = eng->getWorld()->getObject("player/token");
        if(obj) {
            MapBoundsGenerator* mbp = eng->getMapBoundsGenerator();
            if(obj->pos.y < mbp->getOceanUpperBound(obj->pos.x)) {
                core::fp_type thrust = 500 * obj->mass;
                if(this->dirMask & DirUp)
                    obj->forces.push_back(core::Vec3(0, thrust, 0));
                if(this->dirMask & DirDown)
                    obj->forces.push_back(core::Vec3(0, -thrust, 0));
                if(this->dirMask & DirRight)
                    obj->forces.push_back(core::Vec3(thrust, 0, 0));
                if(this->dirMask & DirLeft)
                    obj->forces.push_back(core::Vec3(-thrust, 0, 0));
            }
            core::Object* playerHealth = eng->getWorld()->getObject("player/health");
            
            std::stringstream ss;
            ss << "Health level: " << (idx++);
            dynamic_cast<render::Text*>(playerHealth->renderable)->text = ss.str();
            if(i++ % 5 == 0) {
                ss.str("");
                ss.clear();
                ss << "Velocity: "      << obj->vel << " m/s";
                core::Object* playerSpeed = eng->getWorld()->getObject("player/speed");
                dynamic_cast<render::Text*>(playerSpeed->renderable)->text = ss.str();
                ss.str("");
                ss.clear();
                ss << "Position: "       << obj->pos << " m";
                core::Object* playerPos = eng->getWorld()->getObject("player/pos");
                dynamic_cast<render::Text*>(playerPos->renderable)->text = ss.str();        
                ss.str("");
                ss.clear();
                ss << "Acceleration: "  << obj->acc << " m/s^2";
                core::Object* playerAcc = eng->getWorld()->getObject("player/acc");
                dynamic_cast<render::Text*>(playerAcc->renderable)->text = ss.str();                         
            }
        }
    }

    int getPriority() const 
    {
        return -1;
    }    

    int dirMask;
private:

    DSEngine* eng;
    int idx;
};

/**
 * O(n^2) method of detecting collisions
 * With some magic
 * Affects objects with type == 100 only
 */
struct PlayerBallCollisionDetection : PhysicsProcessor {

    const core::fp_type EPSILON = 0.0f;


    PlayerBallCollisionDetection(DSEngine* e)
        : eng(e)
    {
    }

    inline bool areColliding(const core::Object* left, const core::Object* right)
    {
        if (left != right) {
            core::fp_type diff = (left->pos- right->pos).magnitude()
                                - left->avgRadius 
                                - right->avgRadius;
            return diff <= EPSILON;
        }
        return false;
    }  

    void operator()(core::fp_type delta) {        
        core::Object* obj = eng->getWorld()->getObject("player/token");
        if(obj) {
            std::vector<ds::core::Object*> objs = this->eng->getWorld()->getObjects();
            for (std::vector<ds::core::Object*>::iterator
                    it = objs.begin(),
                    end = objs.end();
                    it != end;
                    ++it) {
                core::Object* other = *it;                
                if(areColliding(obj, other)) {
                    this->eng->getWorld()->remove(other);
                }
            }
        } else {
            std::cout << "Player token not detected" << std::endl;
        }
    }
private:
    DSEngine* eng;
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
    eng->getWorld()->addRenderer(new MapRenderer(eng->getMapBoundsGenerator()));

    //Adding physics effect
    PlayerMovementTracker* playerTracker =
        eng->getPhysicsHandler()->addProcessor(
            new PlayerMovementTracker(eng)
        );

    eng->getPhysicsHandler()->addProcessor(
        new PlayerBallCollisionDetection(eng)
    );
    
    //Adding event handler for player movement
    eng->getEventHandler()->addProcessor(
        new PlayerMovement(playerTracker, eng)
    );

    //Adding event handler for click X event
    eng->getEventHandler()->addProcessor(
        new ExitEventProcessor(eng)
    );

    //Add map to world
    core::Object* map = new core::Object();
    map->name = "World Map";
    map->renderable = new Map();
    eng->getWorld()->add("map", map);

    //Demo code for a test ball
    core::Object* testBall = new core::Object();
    testBall->objectType = 100;
    testBall->avgRadius = 0.25;
    testBall->pos.y = 550;
    testBall->pos.x = 400;    
    testBall->renderable = new render::Sphere();
    testBall->mass = 0.025;

    core::Object* playerHealth = new core::Object();
    core::Object* playerSpeed = new core::Object();
    core::Object* playerPos = new core::Object();
    core::Object* playerAcc = new core::Object();
    
    render::Text* playerHealthText = new render::Text();
    render::Text* playerSpeedText = new render::Text();
    render::Text* playerPosText = new render::Text();
    render::Text* playerAccText = new render::Text();

    playerHealth->pos.x = 5;
    playerHealth->pos.y = 580;
    playerHealthText->style = render::plain12;
    playerHealthText->color = render::Color(0, 0, 255, 255);
    playerHealth->renderable =  playerHealthText;
    playerHealth->getAttrs().set<int>("level", 100);

    playerSpeed->pos.x = 150;
    playerSpeed->pos.y = 580;
    playerSpeedText->style = render::plain12;
    playerSpeedText->color = render::Color(255, 0, 0, 255);
    playerSpeed->renderable =  playerSpeedText;
    
    playerPos->pos.x = 150;
    playerPos->pos.y = 560;
    playerPosText->style = render::plain12;   
    playerPosText->color = render::Color(255, 127, 127, 255);
    playerPos->renderable =  playerPosText;
    
    playerAcc->pos.x = 150;
    playerAcc->pos.y = 540;
    playerAccText->style = render::plain12;   
    playerAccText->color = render::Color(127, 127, 127, 255);
    playerAcc->renderable =  playerAccText;

    eng->getWorld()->add("player/token", testBall);
    eng->getWorld()->add("player/health", playerHealth);
    eng->getWorld()->add("player/speed", playerSpeed);
    eng->getWorld()->add("player/pos", playerPos);
    eng->getWorld()->add("player/acc", playerAcc);

}


}}