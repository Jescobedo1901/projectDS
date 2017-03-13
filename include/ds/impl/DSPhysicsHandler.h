#ifndef DS_IMPL_PHYSICSHANDLER_H
#define DS_IMPL_PHYSICSHANDLER_H

#include <chrono>
#include <thread>
#include "ds/core/World.h"
#include "ds/core/Application.h"
#include "DSHandler.h"

namespace ds { namespace impl {

    struct PhysicsProcessor {
        virtual ~PhysicsProcessor()
        {
        }
        virtual void operator()(core::fp_type delta) = 0;
    };

    struct DSPhysicsHandler : DSHandler<PhysicsProcessor> {

        DSPhysicsHandler();
        void operator()(core::TaskHandlerCondition cond);
        
    };

}}

#endif /* DS_IMPL_PHYSICSHANDLER_H */

