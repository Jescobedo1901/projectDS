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
    void initJacobE(DSEngine* eng);
    //Implemented in marcelF.cpp
    void initMarcelF(DSEngine* eng);
    //Implemented in omarO.cpp
    void initOmarO(DSEngine* eng);
    //Implemented in seanC.cpp
    void initSeanC(DSEngine* eng);

}}

int main (int argc, char** argv) 
{
    std::cout << "Starting application..." << std::endl;

    std::shared_ptr<ds::core::Engine> eng(std::make_shared<ds::impl::DSEngine>());
    std::unique_ptr<ds::core::Application> a(std::make_unique<ds::core::X11Application>(eng));

    //Initialize contributors sections
//    ds::impl::initJacobE(dynamic_cast<ds::impl::DSEngine*>(a->getEngine()));
//    ds::impl::initMarcelF(dynamic_cast<ds::impl::DSEngine*>(a->getEngine()));
//    ds::impl::initOmarO(dynamic_cast<ds::impl::DSEngine*>(a->getEngine()));
//    ds::impl::initSeanC(dynamic_cast<ds::impl::DSEngine*>(a->getEngine()));

    //Blocks until exit is called
    a->run();

    std::cout << "Application terminating..." << std::endl;
    
    return 0;
}
