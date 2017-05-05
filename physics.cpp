#include <complex>

#include "game.h"

//Vec3 implementation - BEGIN

Vec3::Vec3() : x(0), y(0), z(0)
{
}

Vec3::Vec3(float x, float y, float z)
: x(x), y(y), z(z)
{
}

float Vec3::magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::norm() const
{
    float magn = magnitude();
    return Vec3(x / magn, y / magn, z / magn);
}

float Vec3::angleXY() const
{
    return std::atan2(this->y, this->x);
}

Vec3& Vec3::operator+=(const Vec3& r)
{
    x += r.x,
            y += r.y;
    z += r.z;
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& r)
{
    x -= r.x,
            y -= r.y;
    z -= r.z;
    return *this;
}

Vec3& Vec3::operator*=(const Vec3& r)
{
    x *= r.x,
            y *= r.y;
    z *= r.z;
    return *this;
}

Vec3 operator*(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

Vec3 operator*(const float& l, const Vec3& r)
{
    return Vec3(l * r.x, l * r.y, l * r.z);
}

Vec3 operator*(const Vec3& l, const float& r)
{
    return Vec3(l.x * r, l.y * r, l.z * r);
}

Vec3 operator/(const Vec3& l, const float& r)
{
    return Vec3(l.x / r, l.y / r, l.z / r);
}

Vec3 operator/(const float& l, const Vec3& r)
{
    return Vec3(l / r.x, l / r.y, l / r.z);
}

Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3 operator+(const Vec3& l, const float& r)
{
    return Vec3(l.x + r, l.y + r, l.z + r);
}

Vec3 operator+(const float& l, const Vec3& r)
{
    return Vec3(l + r.x, l + r.y, l + r.z);
}

Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vec3 operator-(const Vec3& l, const float& r)
{
    return Vec3(l.x - r, l.y - r, l.z - r);
}

Vec3 operator-(const float& l, const Vec3& r)
{
    return Vec3(l - r.x, l - r.y, l - r.z);
}

Vec3 operator-(const Vec3& v)
{
    return Vec3(-v.x, -v.y, -v.z);
}

Vec3 abs(const Vec3& vec)
{
    return Vec3(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
}

Vec3 sqrt(const Vec3& vec)
{
    return Vec3(std::sqrt(vec.x), std::sqrt(vec.y), std::sqrt(vec.z));
}

// Cross product between two vectors

Vec3 cross(const Vec3& l, const Vec3& r)
{
    return Vec3(
            l.y * r.z - r.y * l.z,
            l.z * r.x - r.z * l.x,
            l.x * r.y - r.x * l.y
            );
}
//Vec3 implementation - END

double diff(timespec& start, timespec& end)
{
    return                  \
        (start.tv_sec - end.tv_sec)
            +
            (double) (start.tv_nsec - end.tv_nsec) * 1e-9;
}

void gameLoop()
{
    const double tickSeconds = 1.0 / 120.0;

    //Used to measure the timer expiration
    struct timespec prev, curr;
    clock_gettime(CLOCK_REALTIME, &prev);

    //temporary calculation results
    struct timespec now;

    while (!game.done) {

        handleEvents();
        
        updateGameStats();

        //tick game state
        clock_gettime(CLOCK_REALTIME, &now);

        double dt = diff(now, prev);

        while (dt >= tickSeconds) {
            dt -= tickSeconds;            
            stepPhysics(tickSeconds);
            applyAudio();
        }

        //render frame
        renderAll();
        elapsedTime();
        prev = now;
    }
}

void stepPhysics(float stepDuration)
{
    /**
     * No physical effects are apply
     * unless the game scene loaded is play
     */
    if (game.scene & GameScenePlay) {

        stepMapBoundsIteration();
        applySpawnRate(stepDuration);
        
        stepFlipBooks(stepDuration);
        
        for (std::vector<Object*>::iterator it = game.objects.begin(), end = game.objects.end(); it != end; ++it) {            
            Object* obj = *it;
            if(applyObjectLifetimePolicies(obj)) {
                continue;
            }
            //Apply to all except object type enemy
            switch(obj->objectType) {
                case ObjectTypeEnemy:
                    if(obj->name == "enemy3") {
                        applyGravity(obj);
                        break;
                    } else if(obj->name == "ship" ||
                            obj->name == "ship-wreck") {
                        goto handleShip;
                    }
                case ObjectTypeFriendly:
                case ObjectTypeNeutral:
                    applyNonPlayerMotion(obj, stepDuration);
                    break; //Just break, don't apply
            handleShip:
            default:
                applyGravity(obj);
                applyStokesApprox(obj);
                applyBuoyancyApprox(obj);
                break;
            }
            applyNewtonianPhysics(obj, stepDuration);
            switch (obj->objectType) {
            case ObjectTypePlayer:
                applyPlayerMovement(obj);
                applyPlayerDirChange(obj);
                applyPlayerOceanBurstMovement(obj, stepDuration);
                break;
            default:
                break;
            }
            applyRotationalHandling(obj, stepDuration);
            applyObjectBoundaryCollision(obj);
        }
        //Broad collision handling separately
        checkObjectCollisions();
    }
}

void applyNewtonianPhysics(Object* obj, float stepDuration)
{
    Vec3 force = obj->cumForces();
    obj->forces.clear(); //clear applied forces

    //Apply only to objects with mass
    if (obj->mass) {
        // a = F / m
        obj->acc = force / obj->mass;

        //Velocity  = acceleration * time
        obj->vel += obj->acc * stepDuration;

        //Position = velocity * time
        // Adjust pixel to meter radio
        obj->pos += obj->vel * stepDuration * PIXEL_TO_METER;
    }
}

void applyStokesApprox(Object* obj)
{
    //Only apply Buoyancy approximation to objects that have mass
    //and if below waves
    int waterLine = getOceanUpperBound(obj->pos.x);
    if (obj->mass &&
            obj->pos.y <= waterLine) {

        //Water Density
        float waterDensity = 1;

        float radius = obj->avgRadius;
        float volEst
                = 4.0 / 3.0 * radius * radius * radius * M_PI;

        //Force of Gravity
        Acceleration forceG(0, -9.8, 0);
        //F_B = V * D * F
        Vec3 forceOfBuoyancy = -(volEst * forceG * waterDensity);
        obj->forces.push_back(forceOfBuoyancy);
    }
}

void applyGravity(Object* obj)
{
    //Apply gravity on objects with mass
    if (obj->mass > 0) {
        obj->forces.push_back(obj->mass * Acceleration(0, -9.8, 0));
    }
}

/**
 * Applies resistance as a product of time
 * http://hyperphysics.phy-astr.gsu.edu/hbase/lindrg.html#c2
 * https://en.wikipedia.org/wiki/Drag_(physics)
 * https://en.wikipedia.org/wiki/Stokes'_law
 */
void applyBuoyancyApprox(Object* obj)
{

    //Only apply Stokes approximation to objects that have mass
    //and if below waves
    if (obj->mass &&
            obj->pos.y <= getOceanUpperBound(obj->pos.x)) {
        //Viscoscity of water at 20C
        float water_mu = 1.002;

        Vec3 force = -(6.0f * (float) (M_PI) * water_mu * obj->avgRadius * obj->vel);

        obj->forces.push_back(force);
    }
}

void stepMapBoundsIteration()
{

    //Step mapBoundsIteration
    //used to move waves forward in time
    ++game.mapBoundsIteration;
}

void applyPlayerMovement(Object* obj)
{
    if (obj->pos.y < getOceanUpperBound(obj->pos.x)) {
        float thrust = game.thrustModifier * obj->mass;
        if (game.playerMovementDirectionMask & DirUp)
            obj->forces.push_back(Vec3(0, thrust, 0));
        if (game.playerMovementDirectionMask & DirDown) {
            obj->forces.push_back(Vec3(0, -(thrust), 0));
        }
        if (game.playerMovementDirectionMask & DirRight) {
            obj->forces.push_back(Vec3(thrust, 0, 0));
        }
        if (game.playerMovementDirectionMask & DirLeft) {
            obj->forces.push_back(Vec3(-(thrust), 0, 0));
        }
    }
}

void applyPlayerDirChange(Object* obj)
{
    if (obj->vel.x > 0 && obj->dim.x > 0) {
        obj->dim.x = -obj->dim.x;
    }
    if (obj->vel.x < 0 && obj->dim.x < 0) {
        obj->dim.x = -obj->dim.x;
    }
}

void applyObjectCollisions(Object* obj)
{
    switch (obj->objectType) {
    case ObjectTypePlayer:
        applyObjectBoundaryCollision(obj);
        break;
    default:
        break;
    }
}

void applyObjectBoundaryCollision(Object* obj)
{
    switch(obj->objectType) {
    case ObjectTypePlayer:
        //Only let player move left left from position game.xres/2
        if(obj->pos.x < game.cameraXMin) {
            obj->pos.x = game.cameraXMin;
            obj->vel.x = -obj->vel.x;
        }
    case ObjectTypeEnemy:
    case ObjectTypeFriendly:
        if (    obj->name != "treasure" &&
                obj->pos.y < getOceanFloorUpperBound(obj->pos.x) + obj->avgRadius * PIXEL_TO_METER) {
            obj->pos.y = getOceanFloorUpperBound(obj->pos.x) + obj->avgRadius * PIXEL_TO_METER;
        }
    default:
        break;
    }
}

void applyPlayerOceanBurstMovement(Object* player, float stepDuration) {
    if (player->pos.y > getOceanUpperBound(player->pos.x) && player->vel.y > 8.0f) {
        player->vel.y = 8.0f;
    }
}

float dimToAvgRadius(Dimension dim)
{
    return (std::abs(dim.x) + std::abs(dim.y)) / 5.0f / PIXEL_TO_METER;
}

float avgRadiusTOEstMass(float avgRadius)
{
    float r = std::abs(avgRadius);
    return 2.5 * 4.0 / 3.0 * r * r * r * M_PI;
}
