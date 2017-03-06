 
#include "ds/core/X11Application.h"
#include "ds/core/Engine.h"

#include <X11/keysym.h>

#include <iostream>
#include <cstdlib> // for atexit func
#include <thread>
#include <vector>


ds::core::X11Application::X11Application(
    std::shared_ptr<ds::core::Engine> eng
    ) : ds::core::Application(eng) {

    if(this->getEngine() == nullptr) {
        throw ApplicationException("Engine cannot be a nullptr");
    }

    GLint glAttr[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

    if(!(this->display= XOpenDisplay(nullptr))) {
        throw ApplicationException("X11: Failed opening X11 display");
    }
    
    this->root= DefaultRootWindow(this->display);
    
    if(!(this->vi = glXChooseVisual(this->display, 0, glAttr))) {
        throw ApplicationException("GL: Acquring visual failed");
    }
    
    this->cmap = XCreateColormap(this->display, this->root, this->vi->visual, AllocNone);
    swa.colormap = this->cmap;
    swa.event_mask = ExposureMask | KeyPressMask;
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

    this->glc = glXCreateContext(this->display, this->vi, nullptr, GL_TRUE);

    XSync(this->display, False);
    
    glXMakeCurrent(this->display, this->win, this->glc);

    glViewport(0, 0, this->gwa.width, this->gwa.height);    

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    glXSwapBuffers(this->display, this->win);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);

    glXSwapBuffers(this->display, this->win);  

    std::cout << "Finished setting up X11 Application.." << std::endl;
    
}

ds::core::X11Application::~X11Application() {
    glXMakeCurrent(this->display, None, NULL);
    glXDestroyContext(this->display, glc);
    XDestroyWindow(this->display, win);
    XCloseDisplay(this->display);
}

void ds::core::X11Application::exit() {
    std::exit(EXIT_SUCCESS);
}

void ds::core::X11Application::nextEvent(XEvent& event) {
    XNextEvent(this->display, &event);
}