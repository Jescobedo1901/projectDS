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

#ifndef DS_IMPL_RENDERINGHANDLER_H
#define DS_IMPL_RENDERINGHANDLER_H

#include <chrono>
#include <thread>
#include <iostream>
#include "ds/core/Application.h"
#include "ds/render/Rendering.h"

namespace ds { namespace impl {


    struct DSEngine; //forward declare
    
    struct DSRenderingHandler {
        
        DSRenderingHandler(DSEngine* e) : eng(e) {}

        void operator()(core::TaskHandlerCondition cond);
    private:
        DSEngine* eng;
    };

}}


#endif /* DS_IMPL_RENDERINGHANDLER_H */

