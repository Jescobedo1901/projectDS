
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSRenderingHandler.h"


void ds::impl::DSRenderingHandler::operator()(core::TaskHandlerCondition cond) {
    while(*cond) {
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        eng->getWorld()->render(eng->getRenderContext());        
        glXSwapBuffers(this->eng->getApplication()->display, this->eng->getApplication()->win);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    std::cout << "I have stopped processing physics" << std::endl;
}
