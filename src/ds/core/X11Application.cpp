#include "ds/core/X11Application.h"
#include "ds/core/Engine.h"
#include "ds/util/utils.h"

#include <X11/keysym.h>
#include <X11/XKBlib.h>

#include <iostream>
#include <cstdlib> // for atexit func
#include <thread>
#include <vector>

ds::core::X11Application::X11Application (std::shared_ptr<Engine> eng)
    :   ds::core::Application (eng),
        display (nullptr),
        root (),
        win (),
        vi (nullptr),
        cmap (),
        swa (),
        gwa (),
        glc (nullptr)
{

    if (this->getEngine() == nullptr) {
        throw ApplicationException("Engine cannot be a nullptr");
    }

    if (!XInitThreads()) {
        throw new ApplicationException("X11 could not initialize threads");
    }

    GLint glAttr[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};

    if (!(this->display = XOpenDisplay(nullptr))) {
        throw ApplicationException("X11: Failed opening X11 display");
    }

    util::XLockDisplayGuard lock(this->display);

    this->root = DefaultRootWindow(this->display);

    if (!(this->vi = glXChooseVisual(this->display, 0, glAttr))) {
        throw ApplicationException("GL: Acquiring visual failed");
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

    XkbSetDetectableAutoRepeat(this->display, 1, nullptr);

    std::cout << "Finished setting up X11 Application.." << std::endl;

}

ds::core::X11Application::~X11Application ()
{
    util::XLockDisplayGuard lock(this->display);
    XDestroyWindow(this->display, win);
    XCloseDisplay(this->display);
}

void ds::core::X11Application::exit ()
{
    std::exit(EXIT_SUCCESS);
}

bool ds::core::X11Application::hasPendingEvents ()
{
    util::XLockDisplayGuard lock(this->display);
    return XPending(this->display) > 0;
}

void ds::core::X11Application::nextEvent (XEvent& event)
{
    util::XLockDisplayGuard lock(this->display);
    XNextEvent(this->display, &event);
}