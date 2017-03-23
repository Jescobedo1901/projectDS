#include "ds/core/X11Application.h"
#include "ds/core/Engine.h"

#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <iostream>
#include <cstdlib> // for atexit func
#include <vector>

ds::core::X11Application::X11Application (Engine* eng)
    :   ds::core::Application (eng),
        display (NULL),
        root (),
        win (),
        vi (NULL),
        cmap (),
        swa (),
        gwa (),
        glc (NULL)
{

    if (this->getEngine() == NULL) {
        init_failure("Engine cannot be a NULL");
    }

    GLint glAttr[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

    if (!(this->display = XOpenDisplay(NULL))) {
        init_failure("X11: Failed opening X11 display");
    }

    this->root = DefaultRootWindow(this->display);

    if (!(this->vi = glXChooseVisual(this->display, 0, glAttr))) {
        init_failure("GL: Acquiring visual failed");
    }

    this->cmap = XCreateColormap(this->display, this->root, this->vi->visual, AllocNone);
    swa.colormap = this->cmap;
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		PointerMotionMask | MotionNotify | ButtonPress | ButtonRelease |
		StructureNotifyMask | SubstructureNotifyMask;
    this->win = XCreateWindow(
        this->display,
        this->root,
        0, 0,
        800,
        600,
        0,
        this->vi->depth,
        InputOutput,
        this->vi->visual,
        CWColormap | CWEventMask,
        &this->swa
    );

    XMapWindow(this->display, this->win);

    XStoreName(this->display, this->win, "Deepsea Survival");

    XkbSetDetectableAutoRepeat(this->display, 1, NULL);
    
    std::cout << "Finished setting up X11 Application.." << std::endl;

}

ds::core::X11Application::~X11Application ()
{
    XDestroyWindow(this->display, win);
    XCloseDisplay(this->display);
}

void ds::core::X11Application::exit ()
{
    std::exit(EXIT_SUCCESS);
}
