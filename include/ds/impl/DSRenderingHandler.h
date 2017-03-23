#ifndef DS_IMPL_RENDERINGHANDLER_H
#define DS_IMPL_RENDERINGHANDLER_H

#include <iostream>
#include "ds/core/Application.h"
#include "ds/render/Rendering.h"
#include "DSHandler.h"

namespace ds { namespace impl {

    struct DSEngine; //forward declare
    
    struct DSRenderingHandler {
        
        DSRenderingHandler(DSEngine* e);
        ~DSRenderingHandler();

        void init();
        void apply();

    private:
        DSEngine* eng;
    };

}}


#endif /* DS_IMPL_RENDERINGHANDLER_H */

