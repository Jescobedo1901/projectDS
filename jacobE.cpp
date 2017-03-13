//Jacob Escobedo
//Group 4
//DeepSea Survival Game

//Marcel Furdui
//Group 4
//DeepSea Survival Game

#include <cmath>
#include <X11/Xlib.h>

#include "ds/core/X11Application.h"
#include "ds/impl/DSEngine.h"
#include "ds/core/World.h"

namespace ds { namespace impl {

//implementations


/**
 * Applies resistance as a product of time (15%)
 * http://hyperphysics.phy-astr.gsu.edu/hbase/lindrg.html#c2
 * https://en.wikipedia.org/wiki/Drag_(physics)
 * https://en.wikipedia.org/wiki/Stokes'_law
 * @param e
 */
struct StokesApprox : PhysicsProcessor {

    StokesApprox (DSEngine* e) : eng (e)
    {
    }

    void operator() (core::fp_type delta)
    {
        auto objs = this->eng->getWorld()->getObjects();
        for (auto it = objs.begin(),
                end = objs.end();
                it != end;
                ++it) {
            
            core::ObjectPtr obj = *it;

            //Viscoscity of water at 20C
            core::fp_type water_mu = 1.002;
            
            auto Force = -(6.0f * static_cast<core::fp_type>(M_PI) * water_mu * obj->avgRadius * obj->vel);

            obj->forces.emplace_back(std::move(Force));
        }
    }

    DSEngine* eng;
};
//handlers

//Initialize handlers

void initJacobE (DSEngine* eng)
{           
    eng->getPhysicsHandler()->addProcessor(std::make_shared<StokesApprox>(eng));    
    //TODO add boyancy effect
}

}}