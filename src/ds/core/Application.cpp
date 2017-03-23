#include "ds/core/Application.h"
#include "ds/core/Engine.h"
#include <iostream>
#include <cstdlib> // for atexit func
#include <vector>
#include <X11/Xlib.h>

ds::core::Application::Application (ds::core::Engine* e)
    :   cond (true),
        eng (e)
{
    if (!this->eng) {
        std::cerr << "Engine pointer was null\n";
        std::exit(1);
    }    
}

void ds::core::Application::init() {
    this->eng->attach(this);
    this->eng->init();
}
ds::core::Application::~Application ()
{
    if (this->eng->getApplication() != this) {
        this->eng->detach(this);
    }
    delete this->eng;
}

void ds::core::Application::run ()
{  
    this->eng->run();
}

ds::core::Engine* ds::core::Application::getEngine ()
{
    return this->eng;
}


void ds::core::init_failure(const char* msg) {
    std::cerr << "An initialization failure occured."
                "With the following message:\n\t" << msg << "\n";
    std::exit(1);
}