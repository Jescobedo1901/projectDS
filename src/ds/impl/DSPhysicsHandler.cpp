
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSPhysicsHandler.h"

ds::impl::DSPhysicsHandler::DSPhysicsHandler ()
    : DSHandler<PhysicsProcessor>()
{
}

void ds::impl::DSPhysicsHandler::apply(core::fp_type delta)
{
    for (std::vector<PhysicsProcessor*>::iterator
            it = this->processors.begin(),
            end = this->processors.end();
            it != end; ++it) {
        (**it)(delta);
    }
}
