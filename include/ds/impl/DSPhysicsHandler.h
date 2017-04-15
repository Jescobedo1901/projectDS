#ifndef DS_IMPL_PHYSICSHANDLER_H
#define DS_IMPL_PHYSICSHANDLER_H

#include "ds/core/World.h"
#include "ds/core/Application.h"
#include "DSHandler.h"

namespace ds { namespace impl {

    struct PhysicsProcessor {
        virtual ~PhysicsProcessor()
        {
        }
        virtual void operator()(core::fp_type delta) = 0;
        
        //Allows physics processor to have priority over others
        //Default priority is 0
        virtual int getPriority() const
        {
            return 0;
        }
    };

    struct DSPhysicsHandler : DSHandler<PhysicsProcessor> {

        DSPhysicsHandler();
        
        void init();
        
        void apply(core::fp_type delta);
        
    };

}}

#endif /* DS_IMPL_PHYSICSHANDLER_H */

