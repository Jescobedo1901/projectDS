//Omar Thor
//Group 4
//DeepSea Survival Game

#include <X11/Xlib.h>

#include "ds/impl/DSEngine.h"
#include "ds/core/World.h"

namespace ds { namespace impl {

    class FishMovementProcessor {
        
        FishMovementProcessor(impl::DSEngine* e) : eng(e) {}

        void operator()(const XEvent& event) {
            if(event.type == KeyPress) {
                if(event.xkey.keycode == XK_Down) {
//                    core::Object* res = eng->getWorld()->find("player");
//                    if(res) {
//                        res->pos.y -= 1;
//                    }
                } else if(event.xkey.keycode == XK_Up) {
//                    core::Object* res = eng->getWorld()->find("player");
//                    if(res) {
//                        res->pos.y += 1;
//                    }
                }
            }
        }

    private:
        ds::impl::DSEngine* eng;
    };

    //implementations
    class MainFishMovement {
        MainFishMovement(impl::DSEngine* e) : eng(e) {}

        void operator()() {

            //core::Object* mf = eng->getWorld()->get("MainFish");
            
           // mf->pos -= 5;
        }

    private:
        ds::impl::DSEngine* eng;
    };


    //handlers
    //These are implementation specific, non-visible classes that change
    //the behavior of the game

    struct ExitEventProcessor {
        ExitEventProcessor(core::X11Application* a)
            :   app(a),
                wmDeleteMessage(
                    XInternAtom(this->app->display, "WM_DELETE_WINDOW", False)
                ) {
            XSetWMProtocols(this->app->display, this->app->win, &wmDeleteMessage, 1);
        }

        void operator()(const XEvent& event) {
            //Handled exit gracefully
            if(event.type == ClientMessage) {
                if(static_cast<unsigned long>(event.xclient.data.l[0]) == wmDeleteMessage) {
                    this->app->exit();
                }
            }
        }
    private:
        ds::core::X11Application * app;
        Atom wmDeleteMessage;        
    };


    /**
     * Initialization hook for Omar's code
     */
    void initOmarO(DSEngine* eng) {
        eng->getEventHandler()->addProcessor(ExitEventProcessor(eng->getApplication()));
        //eng->getRenderingHandler()->addRenderer("Fish_Whale", new WhaleRenderer);
        //eng->getEventHandler()->addProcessor(FishMovement(app));
        //eng->getPhysicsHandler()->addCollsisionDetection(new MainFishMovements);
    }


}}