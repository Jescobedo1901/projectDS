#ifndef DS_IMPL_RENDERINGHANDLER_H
#define DS_IMPL_RENDERINGHANDLER_H

#include <chrono>
#include <thread>
#include <iostream>
#include "ds/core/Application.h"
#include "ds/render/Rendering.h"

namespace ds { namespace impl {


    struct DSEngine; //forward declare
    
    struct DSRenderingHandler {
        
        DSRenderingHandler(DSEngine* e) : eng(e)
        {
        }

        void operator()(core::TaskHandlerCondition cond);
    private:
        DSEngine* eng;
    };

}}


#endif /* DS_IMPL_RENDERINGHANDLER_H */

