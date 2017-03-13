#ifndef DS_CORE_APPLICATION_H
#define DS_CORE_APPLICATION_H

#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <exception>

namespace ds { namespace core {

    //A shared pointer to an atomic boolean on which task handlers depend on
    //Used to signal that their job should no longer be continued
    typedef std::shared_ptr<std::atomic_bool> TaskHandlerCondition;

    typedef std::function<void(TaskHandlerCondition) > TaskHandler;

    /**
     * Manages windowing & system initialization
     */

    class Engine; //Forward declare
    class RenderContext; //Forward declare

    class Application {
    public:

        //Setup application
        Application(std::shared_ptr<Engine> engine);
        //Delete copy constructor and move assignment
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;


        //Takedown application
        virtual ~Application();

        virtual void init();
        
        virtual void run();

        virtual Engine* getEngine();

    protected:
        TaskHandlerCondition cond;
    private:
        std::atomic_bool run_guard;
        std::shared_ptr<Engine> eng;
        std::vector<std::thread> threads;
    };

    //Exceptions

    struct ApplicationException : public std::runtime_error {
        ApplicationException(const char* arg) : runtime_error(arg) 
        {
        }
    };
}}

#endif /* DS_CORE_APPLICATION_H */
