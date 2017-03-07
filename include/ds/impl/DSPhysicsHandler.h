#ifndef DS_IMPL_PHYSICSHANDLER_H
#define DS_IMPL_PHYSICSHANDLER_H

#include <chrono>
#include <thread>
#include "ds/core/Application.h"
#include "DSHandler.h"

namespace ds { namespace impl {

    struct DSPhysicsHandler : DSHandler<std::function<void(double)>> {

        DSPhysicsHandler();
        void operator()(core::TaskHandlerCondition cond);
        
    };

}}

#endif /* DS_IMPL_PHYSICSHANDLER_H */

