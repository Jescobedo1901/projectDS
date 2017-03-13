#ifndef DS_IMPL_EVENTHANDLER_H
#define DS_IMPL_EVENTHANDLER_H

#include <chrono>
#include <thread>
#include <memory>
#include <iostream>
#include <X11/Xlib.h>

#include "DSHandler.h"


namespace ds { namespace impl {


    struct EventProcessor {
        virtual ~EventProcessor()
        {
        }
        virtual void operator()(const XEvent& event) = 0;
    };

    
    struct DSEngine;
    
    struct DSEventHandler
        : public DSHandler<EventProcessor> {
        
        DSEventHandler(DSEngine* eng);

        void operator()(core::TaskHandlerCondition cond);

    private:
        DSEngine* eng;
    };


}}

#endif /* DS_IMPL_EVENTHANDLER_H */

