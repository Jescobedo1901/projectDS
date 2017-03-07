
#ifndef DS_CORE_WORLD_H
#define DS_CORE_WORLD_H

#include <string>
#include <chrono>
#include <memory>
#include <shared_mutex>

//Forward declare render namespace
namespace ds { namespace render {
    class Renderer;
    class Renderable;
    class RenderContext;
}}

namespace ds { namespace core {

    //3 Dimensional vector
    struct Vec3 {
        float x, y, z;
    };
    
    typedef Vec3 Velocity;
    typedef Vec3 Acceleration;
    typedef Vec3 Position;

    typedef std::chrono::high_resolution_clock clock;
    typedef std::chrono::high_resolution_clock::time_point time_point;

    class Object; //forward delcare Object
    typedef std::shared_ptr<Object> ObjectPtr;

    class Application; //forward declare Application

    /**
     * MT safe
     */
    class World {
    public:
            World();
            ~World();
            bool add(ObjectPtr obj);
            bool remove(ObjectPtr obj);
            void addRenderer(render::Renderer* renderer);
            //Render the entire world
            void render(render::RenderContext* ctx);
    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> internal;
    };

    //POD object
    struct Object {
        
        //Name of this object (optional)
        std::string name;

        //Once added to World, world sets itself as owner of Object
        //Pointer to the attached to World object
        //Note, the lifetime of object can extend beyond World
        //Therefore, before accessing world pointer, make sure it points to a
        //valid memory location (i.e. not outside of normal flow)
        World* world;

        //A shared pointer to the renderable that this object represents
        //in a world
        std::shared_ptr<render::Renderable> renderable;

        //The current position of this object
        Position pos;

        //The current velocity of this object
        Velocity vel;
        
        //the current acceleration of this object
        Acceleration acc;

        //When modifying any positional information, this mutex must be held
        std::shared_timed_mutex mutex;
    };


}}


#endif /* DS_CORE_WORLD_H */

