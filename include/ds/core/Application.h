#ifndef DS_CORE_APPLICATION_H
#define DS_CORE_APPLICATION_H

#include <vector>
#include <exception>

namespace ds { namespace core {

    /**
     * Manages windowing & system initialization
     */

    class Engine; //Forward declare
    class RenderContext; //Forward declare

    class Application {
    public:

        //Setup application
        //Application becomes the owner of engine passed
        Application(Engine* engine);

        //Takedown application
        virtual ~Application();

        virtual void init();
        
        virtual void run();

        virtual Engine* getEngine();

    protected:
        bool cond;
    private:
        Engine* eng;
    };
    
    //Cause the application to stop and report an initialization failure
    void init_failure(const char* msg);
}}

#endif /* DS_CORE_APPLICATION_H */
