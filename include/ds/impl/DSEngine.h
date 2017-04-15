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
    
    
    //Stateful map generator that can only move forward
    //Map has concepts of
    struct MapBoundsGenerator : PhysicsProcessor {
        
        int iteration = 0;
        void operator()(core::fp_type delta)
        {
            ++iteration;
        }

        core::fp_type getSkyUpperBound(int x)
        {
            return 600;
        }
        core::fp_type getSkyLowerBound(int x)
        {
            return 450;
        }

        core::fp_type getOceanUpperBound(int x)
        {
            return 500 + 10 * std::sin((x+ iteration*0.25)/25.0);
        }

        core::fp_type getOceanFloorUpperBound(int x) {
            return 100 + 30 * std::sin(x/100.0);
        }

        core::fp_type getOceanFloorLowerBound(int x) {
            return 0;
        }

    };

    const core::fp_type PIXEL_TO_METER = 60.0f;

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
        
        MapBoundsGenerator* getMapBoundsGenerator();
        
    private:                  
        DSEventHandler eventHandler;
        DSPhysicsHandler physicsHandler;
        DSRenderingHandler renderingHandler;
        DSAudioHandler audioHandler;
        
        core::X11Application* app;
        core::World world;
        bool done;
        MapBoundsGenerator* mapBoundsGenerator;
    };
        
    //Newtonian physics model
    struct DSNewtonianPhysics : PhysicsProcessor {
        DSNewtonianPhysics (DSEngine* e);
        void operator() (core::fp_type delta);
        int getPriority() const;

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

