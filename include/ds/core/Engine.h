#ifndef DS_CORE_ENGINE_H
#define DS_CORE_ENGINE_H

#include <memory>
#include <vector>
#include "ds/core/Application.h"

namespace ds { namespace render {
        class RenderContext; //forward declare
}}

namespace ds { namespace core {

    class World;

    class Engine {
    public:

        virtual ~Engine()
        {
        }
        virtual std::vector<TaskHandler> getHandlers() = 0;
        virtual void attach(Application*) = 0;
        virtual void detach(Application*) = 0;
        virtual Application* getApplication() = 0;
        virtual World* getWorld() = 0;
        virtual render::RenderContext* getRenderContext() = 0;
    };


    //Exceptions
    struct EngineException : public std::runtime_error {

        EngineException(const char* arg) : runtime_error(arg)
        {
        }
    };

}}

#endif /* DS_CORE_ENGINE_H */

