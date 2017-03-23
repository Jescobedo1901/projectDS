
#include "ds/core/World.h"
#include "ds/impl/DSEngine.h"
#include "ds/impl/DSRenderingHandler.h"
#include "fonts.h"

ds::impl::DSRenderingHandler::DSRenderingHandler (DSEngine* e)
: eng (e)
{   
}

void ds::impl::DSRenderingHandler::init()
{
    verify(this->eng);
    if (!(this->eng->getApplication()->glc = glXCreateContext(
            this->eng->getApplication()->display,
            this->eng->getApplication()->vi,
            NULL,
            GL_TRUE))
        ) {
        core::init_failure("Failed creating new GLX context");
    }

    if (!glXMakeCurrent(
            this->eng->getApplication()->display,
            this->eng->getApplication()->win,
            this->eng->getApplication()->glc)
            ) {
        core::init_failure("Failed making GLX current");
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

    glEnable(GL_MULTISAMPLE);

    //Clear the screen to black
    glClearColor(0.0, 0.0, 0.0, 1.0);

    initialize_fonts();        
}
ds::impl::DSRenderingHandler::~DSRenderingHandler ()
{
    cleanup_fonts();
    glXMakeCurrent(this->eng->getApplication()->display, None, NULL);
    glXDestroyContext(this->eng->getApplication()->display, this->eng->getApplication()->glc);
}

void ds::impl::DSRenderingHandler::apply()
{
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer (background)
    eng->getWorld()->render(this->eng->getRenderContext());
    glXSwapBuffers(this->eng->getApplication()->display, this->eng->getApplication()->win);
}
