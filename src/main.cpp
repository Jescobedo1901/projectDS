// CMPS 3350 - Project: Deepsea Survival
// Authors:
//  Chris
//  Jake
//  Omar
//  Sean

#include <iostream>
#include <memory>
#include "ds/core/Application.h"
#include "ds/core/X11Application.h"
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"

// Initialization code
// Seperates all implementations and decouples
namespace ds { namespace impl {

    //Implemented in jacobE.cpp
    void initJacobE (DSEngine* eng);
    //Implemented in marcelF.cpp
    void initMarcelF (DSEngine* eng);
        //Implemented in omarO.cpp
    void initOmarO (DSEngine* eng);
    //Implemented in seanC.cpp
    void initSeanC (DSEngine* eng);

}}

int main (int argc, char** argv)
{
    std::cout << "Starting application..." << std::endl;

    
    using namespace ds::impl;
    using namespace ds::core;
    

    DSEngine* e = new DSEngine();
    X11Application app(e);
    app.init();
    
    //Initialize contributors sections
    ds::impl::initJacobE(e);
    ds::impl::initMarcelF(e);
    ds::impl::initOmarO(e);
    ds::impl::initSeanC(e);

    //Blocks until exit is called
    app.run();

    std::cout << "Application terminating..." << std::endl;

    return 0;
}
