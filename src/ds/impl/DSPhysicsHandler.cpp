
#include <chrono>
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSPhysicsHandler.h"

ds::impl::DSPhysicsHandler::DSPhysicsHandler ()
    : DSHandler<PhysicsProcessor>()
{
}

void ds::impl::DSPhysicsHandler::operator() (core::TaskHandlerCondition cond)
{
    util::Timer<60, true> timer;
    timer.run([&]
    {
        auto delta = timer.getDelta();
        for (auto it = this->processors.begin(),
                end = this->processors.end();
                it != end; ++it) {
            (**it)(delta);
        }
    }, *cond);

    std::cout << "I have stopped processing physics" << std::endl;
}
