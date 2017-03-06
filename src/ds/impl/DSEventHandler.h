
#ifndef DS_IMPL_EVENTHANDLER_H
#define DS_IMPL_EVENTHANDLER_H

#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <X11/Xlib.h>
#include <initializer_list>
#include "DSHandler.h"

namespace ds { namespace impl {

    struct DSEventHandler : public DSHandler<std::function<void(const XEvent&)>> {
        DSEventHandler(
            core::X11Application* appli)
                : DSHandler(), app(appli) {}
        
        void operator()(core::TaskHandlerCondition cond) {
            std::cout << "I am starting to process events" << std::endl;
            XEvent event;
            while (*cond) {
                    //Process events
                    this->app->nextEvent(event);
                    for(auto    it = this->processors.begin(),
                                end = this->processors.end();
                                it != end;
                                ++it) {
                        (*it)(event);
                    }

                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }

            std::cout << "I have stopped processing events" << std::endl;
        }

    private:
        ds::core::X11Application* app;

    };

    
}}

#endif /* DS_IMPL_SDLHANDLER_H */

