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

void handleEvents()
{
    XEvent event;
    while (XPending(game.display) > 0) {
        XNextEvent(game.display, &event);
        handleWindowResize(event);
        handlePlayerMovement(event);
        handleMenuMouseMovement(event);
        handlePlayerClickExit(event);
        handleESC(event);
        handleMenuPress(event);
        handleUpgradePress(event);
        handleMouseClicks(event);
        audioLoop();
    }
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

void applyPlayerDirChange(Object* obj)
{
    if (obj->vel.x > 0 && obj->dim.x > 0) {
        obj->dim.x = -obj->dim.x;
    }
    if (obj->vel.x < 0 && obj->dim.x < 0) {
        obj->dim.x = -obj->dim.x;
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
