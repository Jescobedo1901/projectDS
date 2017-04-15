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
        MapBoundsGenerator* mbp = eng->getMapBoundsGenerator();
        std::vector<ds::core::Object*> objs = this->eng->getWorld()->getObjects();
        for (std::vector<ds::core::Object*>::iterator
                it = objs.begin(),
                end = objs.end();
                it != end;
                ++it) {
            core::Object* obj = *it;

            //Only apply Stokes approximation to objects that have mass
            //and if below waves
            if(     obj->mass &&
                    obj->pos.y <= mbp->getOceanUpperBound(obj->pos.x)) {
                //Viscoscity of water at 20C
                core::fp_type water_mu = 1.002;

                core::Vec3 force = -(6.0f * static_cast<core::fp_type>(M_PI) * water_mu * obj->avgRadius * obj->vel);

                obj->forces.push_back(force);
            }
        }
    }

    DSEngine* eng;
};

struct BuoyancyApprox : PhysicsProcessor {

    BuoyancyApprox (DSEngine* e) : eng (e)
    {
    }

    void operator() (core::fp_type delta)
    {
        MapBoundsGenerator* mbp = eng->getMapBoundsGenerator();
        std::vector<ds::core::Object*> objs = this->eng->getWorld()->getObjects();
        for (std::vector<ds::core::Object*>::iterator
                it = objs.begin(),
                end = objs.end();
                it != end;
                ++it) {
            core::Object* obj = *it;

            //Only apply Buoyancy approximation to objects that have mass
            //and if below waves
            int waterLine = mbp->getOceanUpperBound(obj->pos.x);
            if(     obj->mass &&
                    obj->pos.y <= waterLine) {
                //Viscoscity of water at 20C
                core::fp_type waterDensity = 1;

                core::fp_type radius = obj->avgRadius;
                core::fp_type volEst
                        =  4.0/3.0 * radius * radius * radius * M_PI;

                core::Vec3 forceOfGravity(0, -9.8, 0);
                //F_B = V * D * F
                core::Vec3 forceOfBuoyancy =  -(volEst *
                                                forceOfGravity *
                                                waterDensity);

                obj->forces.push_back(forceOfBuoyancy);
            }
        }
    }

    DSEngine* eng;
};
//handlers

//Initialize handlers
void initJacobE (DSEngine* eng)
{
    eng->getPhysicsHandler()->addProcessor(new StokesApprox(eng));
    eng->getPhysicsHandler()->addProcessor(new BuoyancyApprox(eng));
}

}}