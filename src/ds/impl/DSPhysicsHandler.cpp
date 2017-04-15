

#include <algorithm>

#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSPhysicsHandler.h"

ds::impl::DSPhysicsHandler::DSPhysicsHandler ()
    : DSHandler<PhysicsProcessor>()
{
}

struct PhysicsProcessorComparator {
    bool operator()(
        const ds::impl::PhysicsProcessor* left, 
        const ds::impl::PhysicsProcessor* right) const {
        return left->getPriority() < right->getPriority();
    }
};
void ds::impl::DSPhysicsHandler::init()
{
    std::sort(
        this->processors.begin(), 
        this->processors.end(), 
        PhysicsProcessorComparator()
    );
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
