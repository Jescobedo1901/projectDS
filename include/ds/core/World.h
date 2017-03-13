#ifndef DS_CORE_WORLD_H
#define DS_CORE_WORLD_H

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <cmath>
#include <shared_mutex>
#include <iosfwd>

//Forward declare render namespace
namespace ds { namespace render {
    class Renderer;
    class Renderable;
    class RenderContext;
}}

namespace ds { namespace core {

    typedef float fp_type;
    //3 Dimensional vector
    struct Vec3 {
        fp_type x, y, z;
        
        float magnitude() {
            return cbrtf(x*x + y*y + z*z);
        }

        inline Vec3& operator+=(const Vec3& r) {
            this->x += r.x,
            this->y += r.y;
            this->z += r.z;
            return *this;
        }

        inline Vec3& operator-=(const Vec3& r) {
            this->x -= r.x,
            this->y -= r.y;
            this->z -= r.z;
            return *this;
        }

        inline Vec3& operator *=(const Vec3& r) {
            this->x *= r.x,
            this->y *= r.y;
            this->z *= r.z;
            return *this;
        }
       
    };

    //Simple math operators

    inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
        return Vec3{v1.x * v2.x, v1.y * v2.y, v1.z * v2.z};
    }

    template<typename T>
    inline Vec3 operator*(const T& l, const Vec3& r) {
        return Vec3{l * r.x, l * r.y, l * r.z};
    }

    template<typename T>
    inline Vec3 operator*(const Vec3& l, const T& r) {
        return Vec3{l.x * r, l.y * r, l.z * r};
    }

    template<typename T>
    inline Vec3 operator/(const Vec3& l, const T& r) {
        return Vec3{l.x / r, l.y / r, l.z / r};
    }

    template<typename T>
    inline Vec3 operator/(const T& l, const Vec3& r) {
        return Vec3{l / r.x, l / r.y, l / r.z};
    }

    inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
        return Vec3{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
    }
    
    template<typename T>
    inline Vec3 operator+(const Vec3& l, const T& r) {
        return Vec3{l.x + r, l.y + r, l.z + r};
    }

    template<typename T>
    inline Vec3 operator+(const T& l, const Vec3& r) {
        return Vec3{l + r.x, l + r.y, l + r.z};
    }

    inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
        return Vec3{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
    }

    template<typename T>
    inline Vec3 operator-(const Vec3& l, const T& r) {
        return Vec3{l.x - r, l.y - r, l.z - r};
    }

    template<typename T>
    inline Vec3 operator-(const T& l, const Vec3& r) {
        return Vec3{l - r.x, l - r.y, l - r.z};
    }        

    inline Vec3 operator-(const Vec3& v) {
        return Vec3{-v.x, -v.y, -v.z};
    }

    std::ostream& operator<<(std::ostream& os, const Vec3 & v);
    
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
            void addRenderer(std::shared_ptr<render::Renderer> renderer);
            //Render the entire world
            void render(render::RenderContext* ctx);

            /**
             * Returns a copy of all objects in the world
             * @return
             */
            std::vector<ObjectPtr> getObjects();            
    private:
        struct Pimpl;
        std::unique_ptr<Pimpl> internal;
    };

    //POD object
    struct Object {

        Object()
            :   name(), world(nullptr), renderable(),
                pos(), vel(), acc(), avgRadius(),
                mass(), mutex()         
        {
        }
        
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

        //The average radius of the object
        //Not necessarily a meaningful attribute
        fp_type avgRadius;

        //The mass of the object (kg)
        fp_type mass;

        // A vector of vector forces being applied to this object
        // Must be reapplied every step in order to have continuous effect
        std::vector<Vec3> forces;

        Vec3 cumForces() {
            Vec3 cum{0,0,0};
            for(auto& f : forces) {
                cum.x += f.x;
                cum.y += f.y;
                cum.z += f.z;
            }
            return cum;
        }

        //When modifying any positional information, this mutex must be held
        std::shared_timed_mutex mutex;
    };

}}


#endif /* DS_CORE_WORLD_H */

