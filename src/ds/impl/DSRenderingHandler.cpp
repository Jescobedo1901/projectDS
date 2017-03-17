
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSRenderingHandler.h"
#include "ds/util/utils.h"
#include "fonts.h"

ds::impl::DSRenderingHandler::DSRenderingHandler (DSEngine* e)
: eng (e)
{    
}

void ds::impl::DSRenderingHandler::operator() (core::TaskHandlerCondition cond)
{
    verify(eng);
    {
        util::XLockDisplayGuard guard(this->eng->getApplication()->display);
        //Move rendering context to the current thread

        if (!(this->eng->getApplication()->glc = glXCreateContext(
                this->eng->getApplication()->display,
                this->eng->getApplication()->vi,
                nullptr,
                GL_TRUE))
            ) {
            throw core::EngineException("Failed creating new GLX context");
        }

        if (!glXMakeCurrent(
                this->eng->getApplication()->display,
                this->eng->getApplication()->win,
                this->eng->getApplication()->glc)
                ) {
            throw core::EngineException("Failed making GLX current");
        }

        glViewport(
            0, 0,
            this->eng->getApplication()->gwa.width,
            this->eng->getApplication()->gwa.height
        );

        XGetWindowAttributes(
            this->eng->getApplication()->display,
            this->eng->getApplication()->win,
            &this->eng->getApplication()->gwa
        );

        std::cout   << "Width is: " << this->eng->getApplication()->gwa.width
                    << ", and height is: "
                    << this->eng->getApplication()->gwa.width
                    << std::endl;
        glOrtho(
            0,
            this->eng->getApplication()->gwa.width,
            0,
            this->eng->getApplication()->gwa.height,
            -1,
            1
        );

        glViewport(
            0, 0,
            this->eng->getApplication()->gwa.width,
            this->eng->getApplication()->gwa.height
        );
        //Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();

        //This sets 2D mode (no perspective)
	glOrtho(
            0,
            this->eng->getApplication()->gwa.width,
            0,
            this->eng->getApplication()->gwa.height,
            -1,
            1
        );

        glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	glDisable(GL_CULL_FACE);
        
        glEnable(GL_TEXTURE_2D);

        //Clear the screen to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
        
        initialize_fonts();    
    }

    util::Timer<60, false> timer;
            timer.run(
            [&]
            {

                glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)

                eng->getWorld()->render(eng->getRenderContext());

                util::XLockDisplayGuard guard(this->eng->getApplication()->display);
                glXSwapBuffers(this->eng->getApplication()->display, this->eng->getApplication()->win);

            }, *cond);

    {
        cleanup_fonts();
        util::XLockDisplayGuard guard(this->eng->getApplication()->display);
        glXMakeCurrent(this->eng->getApplication()->display, None, NULL);
        glXDestroyContext(this->eng->getApplication()->display, this->eng->getApplication()->glc);
    }

}
