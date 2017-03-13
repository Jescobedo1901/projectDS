#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <X11/Xlib.h>

#include "ds/impl/DSEngine.h"
#include "ds/impl/DSEventHandler.h"
#include "ds/impl/DSHandler.h"

ds::impl::DSEventHandler::DSEventHandler (DSEngine* e)
    : eng (e)
{
}

void ds::impl::DSEventHandler::operator() (core::TaskHandlerCondition cond)
{
    std::cout << "I am starting to process events" << std::endl;
    XEvent event;

    if (!cond) {
        std::cout   << "The condition task handler condition doesn't "
                    << "point to anything.... C'mon!!!" << std::endl;
    }
    verify(this->eng);
    while (*cond) {
        //Process events
        while (this->eng->getApplication()->hasPendingEvents()) {
            this->eng->getApplication()->nextEvent(event);
//            if(event.type == Expose) {
//                util::XLockDisplayGuard guard(this->eng->getApplication()->display);
//                XGetWindowAttributes(
//                    this->eng->getApplication()->display,
//                    this->eng->getApplication()->win,
//                    &this->eng->getApplication()->gwa
//                );
//                auto    width = this->eng->getApplication()->gwa.width,
//                        height = this->eng->getApplication()->gwa.height;
//                glMatrixMode(GL_PROJECTION); glLoadIdentity();
//                glMatrixMode(GL_MODELVIEW); glLoadIdentity();
//                glOrtho(0, width, 0, height, -1, 1);
//                glViewport(0, 0, width, height);
//            }
            for (auto it = this->processors.begin(),
                    end = this->processors.end();
                    it != end;
                    ++it) {
                (**it)(event);
            }
        }
    }

    std::cout << "I have stopped processing events" << std::endl;
}
