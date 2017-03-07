#ifndef DS_IMPL_AUDIOHANDLER_H
#define DS_IMPL_AUDIOHANDLER_H

#include <chrono>
#include <thread>
#include "ds/core/Application.h"

namespace ds { namespace impl {

    struct DSAudioHandler {
        
        void operator()(core::TaskHandlerCondition cond)
        {
            while (*cond) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::cout << "I have stopped processing audio" << std::endl;
        }
        
    };

}}

#endif /* DS_IMPL_AUDIOHANDLER_H */

