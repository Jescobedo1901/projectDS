#ifndef DS_IMPL_ENGINE_H
#define DS_IMPL_ENGINE_H

#include <vector>

#include "ds/core/Engine.h"
#include "ds/core/X11Application.h"

#include "DSEventHandler.h"
#include "DSPhysicsHandler.h"
#include "DSRenderingHandler.h"
#include "DSAudioHandler.h"

namespace ds { namespace impl {

    const core::fp_type PIXEL_TO_METER = 4;

    class DSEngine : public core::Engine, public render::RenderContext {
    public:
        DSEngine();
        ~DSEngine();

        void verify();

        void attach(core::Application*);

        void detach(core::Application*);

        core::X11Application* getApplication();

        core::World* getWorld();
        
        void init();
        void run();

        DSAudioHandler* getAudioHandler();

        DSEventHandler* getEventHandler();

        DSPhysicsHandler* getPhysicsHandler();

        DSRenderingHandler* getRenderingHandler();
        
        render::RenderContext* getRenderContext();

    private:                  
        DSEventHandler eventHandler;
        DSPhysicsHandler physicsHandler;
        DSRenderingHandler renderingHandler;
        DSAudioHandler audioHandler;
        
        core::X11Application* app;
        core::World world;
        bool done;
    };
        
    //Newtonian physics model
    struct DSNewtonianPhysics : PhysicsProcessor {
        DSNewtonianPhysics (DSEngine* e);
        void operator() (core::fp_type delta);
    private:
        DSEngine* eng;
    };
    
    //Simple gravity model
    struct DSGravity : PhysicsProcessor {
        DSGravity (core::Vec3 acc, DSEngine* e);
        void operator() (core::fp_type delta);    
    private:
        core::Vec3 acceleration;
        DSEngine* eng;
    };

    inline void verify(DSEngine* eng) {
        if (!eng) {
            ds::core::init_failure("Engine not initialized");
        }
        if (!eng->getApplication()) {
            ds::core::init_failure("Engine is not attached");
        }
        if (!eng->getWorld()) {
            ds::core::init_failure("World is not ready");
        }
    }

}}


#endif /* DS_IMPL_ENGINE_H */

