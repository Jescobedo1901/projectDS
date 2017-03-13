#include "ds/core/Application.h"
#include "ds/core/Engine.h"
#include <iostream>
#include <cstdlib> // for atexit func
#include <vector>
#include <X11/Xlib.h>

ds::core::Application::Application (std::shared_ptr<ds::core::Engine> e)
    :   cond (std::make_shared<std::atomic_bool>(true)),
        eng (e),
        threads ()
{
    if (!this->eng) {
        throw ApplicationException("Invalid engine pointer");
    }    
}

void ds::core::Application::init() {
    this->eng->attach(this);
}
ds::core::Application::~Application ()
{
    (*this->cond) = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    if (this->eng->getApplication() == this) {
        this->eng->detach(this);
    }
}

void ds::core::Application::run ()
{
    auto engHandlers = this->eng->getHandlers();
    for (auto it = engHandlers.begin(),
            end = engHandlers.end(); it != end; ++it) {
        //create new thread and put it into our container
        this->threads.emplace_back(*it, std::ref(this->cond));
    }
    for (auto& it : this->threads) {
        it.join();
    }
    this->eng->detach(this);
}

ds::core::Engine* ds::core::Application::getEngine ()
{
    return this->eng.get();
}
