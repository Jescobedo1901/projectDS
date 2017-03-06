/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Engine.h
 * Author: thor
 *
 * Created on February 22, 2017, 7:08 PM
 */

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
        virtual ~Engine() {}
        virtual std::vector<TaskHandler> getHandlers() = 0;
        virtual void attach(Application*) = 0;
        virtual void detach(Application*) = 0;
        virtual Application* getApplication() = 0;
        virtual World* getWorld() = 0;
        virtual render::RenderContext* getRenderContext() = 0;
    };


    //Exceptions
    struct EngineException : public std::runtime_error {
        EngineException(const char* arg) : runtime_error(arg) {}
    };

}}

#endif /* DS_CORE_ENGINE_H */

