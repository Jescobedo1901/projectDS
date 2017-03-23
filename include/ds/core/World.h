#ifndef DS_CORE_WORLD_H
#define DS_CORE_WORLD_H

#include <string>
#include <vector>
#include <cmath>
#include <iosfwd>
#include <map>

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

        Vec3() : x(0), y(0), z(0)
        {
        }

        Vec3(fp_type x, fp_type y, fp_type z)
            : x(x), y(y), z(z)
        {
        }

        fp_type x, y, z;

        core::fp_type magnitude() {
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

            return *this;
        }      

    };

    //Simple math operators

    inline Vec3 operator*(const Vec3& v1, const Vec3& v2) {
        return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
    }

    template<typename T>
    inline Vec3 operator*(const T& l, const Vec3& r) {
        return Vec3(l * r.x, l * r.y, l * r.z);
    }

    template<typename T>
    inline Vec3 operator*(const Vec3& l, const T& r) {
        return Vec3(l.x * r, l.y * r, l.z * r);
    }

    template<typename T>
    inline Vec3 operator/(const Vec3& l, const T& r) {
        return Vec3(l.x / r, l.y / r, l.z / r);
    }

    template<typename T>
    inline Vec3 operator/(const T& l, const Vec3& r) {
        return Vec3(l / r.x, l / r.y, l / r.z);
    }

    inline Vec3 operator+(const Vec3& v1, const Vec3& v2) {
        return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    template<typename T>
    inline Vec3 operator+(const Vec3& l, const T& r) {
        return Vec3(l.x + r, l.y + r, l.z + r);
    }

    template<typename T>
    inline Vec3 operator+(const T& l, const Vec3& r) {
        return Vec3(l + r.x, l + r.y, l + r.z);
    }

    inline Vec3 operator-(const Vec3& v1, const Vec3& v2) {
        return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    template<typename T>
    inline Vec3 operator-(const Vec3& l, const T& r) {
        return Vec3(l.x - r, l.y - r, l.z - r);
    }

    template<typename T>
    inline Vec3 operator-(const T& l, const Vec3& r) {
        return Vec3(l - r.x, l - r.y, l - r.z);
    }

    inline Vec3 operator-(const Vec3& v) {
        return Vec3(-v.x, -v.y, -v.z);
    }
    
        
    inline Vec3 abs(const Vec3& vec) {
        return Vec3(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
    }
    
    inline Vec3 sqrt(const Vec3& vec) {
        return Vec3(std::sqrt(vec.x), std::sqrt(vec.y), std::sqrt(vec.z));
    }

    std::ostream& operator<<(std::ostream& os, const Vec3 & v);

    typedef Vec3 Velocity;
    typedef Vec3 Acceleration;
    typedef Vec3 Position;

    class Object; //forward delcare Object

    class Application; //forward declare Application

    class World {
    public:
        typedef std::string ObjectKeyType;
            World();
            ~World();
            //Adding Object to World
            //Passing ownership to World
            bool add(const ObjectKeyType& key, Object* obj);

            //Removes AND deletes the object from the world
            //also deletes the object itself
            //Therefore the pointer is invalid after this call
            bool remove(Object* obj);
            //Removes object by key
            //Any references to the Object are invalid after this call
            bool remove(const ObjectKeyType& key);

            //Add renderer to World, passing ownership to World
            void addRenderer(render::Renderer* renderer);

            //Render the entire world
            void render(render::RenderContext* ctx);

            /**
             * Returns a copy of all objects pointers in the world
             * @return
             */
            std::vector<Object*> getObjects();

            /**
             * Returns a copy of the objects pointers
             * @param key
             * @return
             */
            Object* getObject(const ObjectKeyType& key);

    private:
        struct Pimpl;
        Pimpl* internal;
    };


    struct Attribute {
        virtual ~Attribute()
        {
        }
    };

    //Value holder for attributes (for type T)
    template<typename T>
    struct TypedAttribute : Attribute {
        TypedAttribute(T value) : value(value)
        {
        }

        inline T& get()
        {
            return value;
        }

        inline void set(T value)
        {
            this->value = value;
        }
    private:
        T value;
    };


    template<typename KeyType = std::string>
    struct AttributeMap {

        typedef KeyType key_type;

        AttributeMap() : attrs()
        {
        }

        virtual ~AttributeMap() {
            for(typename std::map<KeyType, Attribute*>::iterator
                    it = this->attrs.begin(),
                    end = this->attrs.end();
                    it != end;
                    ++it) {
                delete (*it).second;
            }
        }

        template<typename T>
        bool set(const KeyType& key, T value)
        {
            typename std::map<KeyType, Attribute*>::iterator
                    it = this->attrs.find(key);

            if(it == attrs.end()) {
                this->attrs[key] = new TypedAttribute<T>(value);
                return true;
            }
            TypedAttribute<T>* ptr =
                    dynamic_cast<TypedAttribute<T>*>((*it).second);
            if(ptr) {
                ptr->set(value);
                return true;
            }
            return false;
        }

        /**
         * Get value from map by key
         * If key does not exist, a NULL is returned
         * If type specified is wrong, this call will simply return a
         * NULL value
         * @param key
         * @return
         */
        template<typename T>
        T* get(const KeyType& key) const
        {
            typename std::map<KeyType, Attribute*>::iterator
                res = this->attrs.find(key);
            if(res != attrs.end()) {
                TypedAttribute<T>* ptr =
                        dynamic_cast<TypedAttribute<T>*>((*res).second);
                if(ptr) {
                    return &ptr->get();
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
        }

        template<typename T>
        T* getOrDefault(const KeyType& key, T& def) const
        {
            T* res = get(key);
            if(res) {
                return res;
            } else {
                return &def;
            }
        }

    private:
        std::map<KeyType, Attribute*> attrs;
    };


    /**
     * MT Safe with the proper usage of APIs
     * All changes to this object's state must be through
     * the transaction macros, or manually locking the mutex
     * See DS_SCOPED_OBJECT_WRITE_LOCK and DS_SCOPED_OBJECT_READ_LOCK
     */
    struct Object {

        Object()
            :   name(), objectType(-1), world(NULL), renderable(NULL),
                pos(), vel(), acc(), avgRadius(),
                mass(), forces(0),
                attributes()
        {
        }

        //Name of this object (optional)
        std::string name;
        
        //Integer object type identifier (optional)
        int objectType;

        //Once added to World, world sets itself as owner of Object
        //Pointer to the attached to World object
        //Note, the lifetime of object can extend beyond World
        //Therefore, before accessing world pointer, make sure it points to a
        //valid memory location (i.e. not outside of normal flow)
        World* world;

        //A shared pointer to the renderable that this object represents
        //in a world
        render::Renderable* renderable;

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

        inline Vec3 cumForces() const {
            Vec3 cum;
            if(!forces.empty()) {
                for(std::vector<Vec3>::const_iterator
                        it = forces.begin(),
                        end = forces.end();
                        it != end;
                        ++it) {
                    const Vec3& f= *it;
                    cum.x += f.x;
                    cum.y += f.y;
                    cum.z += f.z;
                }
            }
            return cum;
        }

        AttributeMap<>& getAttrs() {
            return this->attributes;
        }

    private:

        Object(const Object&)
        {
        }
        Object& operator=(const Object&)
        {
            return *this;
        }
        //When modifying any positional information, this mutex must be held

        AttributeMap<> attributes;
    };   

}}


#endif /* DS_CORE_WORLD_H */

