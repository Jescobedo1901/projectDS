/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   DSEngine.h
 * Author: thor
 *
 * Created on February 22, 2017, 3:42 PM
 */

#ifndef DS_IMPL_PHYSICSHANDLER_H
#define DS_IMPL_PHYSICSHANDLER_H

#include <chrono>
#include <thread>
#include "ds/core/Application.h"

namespace ds { namespace impl {

    struct DSPhysicsHandler {

        void operator()(core::TaskHandlerCondition cond) {
            while(*cond) {
                std::cout << "I am processing physics" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
            std::cout << "I have stopped processing physics" << std::endl;
        }

        

    };

}}

#endif /* DS_IMPL_PHYSICSHANDLER_H */

