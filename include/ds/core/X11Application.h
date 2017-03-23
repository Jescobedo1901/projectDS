#ifndef DS_CORE_X11APPLICATION_H
#define DS_CORE_X11APPLICATION_H

#include<cstdio>
#include<cstdlib>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

#include "Application.h"

namespace ds { namespace core {

    class Engine; //Forward declare

    class X11Application : public Application {
    public:

        //Setup application
        X11Application(Engine* engine);

        ~X11Application();

        //Generic API
        void exit();

    public:
        Display* display;
        Window root;
        Window win;
        XVisualInfo* vi;
        Colormap cmap;
        XSetWindowAttributes swa;
        XWindowAttributes gwa;
        GLXContext glc;
    };

}}

#endif /* DS_CORE_X11APPLICATION_H */
