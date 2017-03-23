
#include <iostream>
#include <X11/Xlib.h>

#include "ds/impl/DSEngine.h"
#include "ds/impl/DSEventHandler.h"
#include "ds/impl/DSHandler.h"

ds::impl::DSEventHandler::DSEventHandler (DSEngine* e)
    : eng (e)
{
}

void ds::impl::DSEventHandler::apply()
{
    XEvent event;     
    while (XPending(this->eng->getApplication()->display) > 0) {
        XNextEvent(this->eng->getApplication()->display, &event);
        for (std::vector<EventProcessor*>::iterator
                it = this->processors.begin(),
                end = this->processors.end();
                it != end;
                ++it) {
            (**it)(event);
        }
    }
}
