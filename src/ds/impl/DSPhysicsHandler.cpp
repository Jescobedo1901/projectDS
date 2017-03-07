
#include <chrono>
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSPhysicsHandler.h"

ds::impl::DSPhysicsHandler::DSPhysicsHandler()
    : DSHandler<std::function<void(double)>>() {}



void ds::impl::DSPhysicsHandler::operator ()(core::TaskHandlerCondition cond) {

    util::Timer<360, true> timer;
    timer.run([&] {
        for(auto it = this->processors.begin(),
                 end = this->processors.end();
                 it != end; ++it) {
            (*it)(0.0);
        }
    }, *cond);
    
    std::cout << "I have stopped processing physics" << std::endl;
}