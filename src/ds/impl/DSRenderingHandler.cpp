
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSRenderingHandler.h"
#include "ds/util/utils.h"

void ds::impl::DSRenderingHandler::operator() (core::TaskHandlerCondition cond)
{
    {
        util::XLockDisplayGuard guard(this->eng->getApplication()->display);
        //Move rendering context to the current thread
        this->eng->getApplication()->glc = glXCreateContext(
            this->eng->getApplication()->display,
            this->eng->getApplication()->vi,
            nullptr,
            GL_TRUE
        );
        glXMakeCurrent(
            this->eng->getApplication()->display,
            this->eng->getApplication()->win,
            this->eng->getApplication()->glc
        );
        glViewport(
            0, 0,
            this->eng->getApplication()->gwa.width,
            this->eng->getApplication()->gwa.height
        );
    }

    util::Timer<60, false> timer;
    timer.run(
        [&]
        {
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
            util::XLockDisplayGuard guard(
                    this->eng->getApplication()->display
                    );
            //eng->getWorld()->render(eng->getRenderContext());
            glXSwapBuffers(this->eng->getApplication()->display, this->eng->getApplication()->win);
        }, *cond);

    {
        util::XLockDisplayGuard guard(this->eng->getApplication()->display);
        glXMakeCurrent(this->eng->getApplication()->display, None, NULL);
        glXDestroyContext(this->eng->getApplication()->display, this->eng->getApplication()->glc);
    }

}
