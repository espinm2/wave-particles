#ifndef WALL_H
#define WALL_H

#include "vectors.h"
#include <cmath>

class Wall
{
public:

    // Wall(const Vec3f &a, const Vec3f &b, const Vec3f &dir);
    Wall(Vec3f a, Vec3f b, Vec3f dir);

    // Getter fuctions
    /*
    Vec3f getA() const;
    Vec3f getB() const;
    Vec3f getIndoorDir() const;
    */

    Vec3f getA() const;
    Vec3f getB() const;
    Vec3f getIndoorDir() const;

    // Setter functions
    void setA(const Vec3f &value);
    void setB(const Vec3f &value);
    void setIndoorDir(const Vec3f &value);


    // useful functions
    Vec3f getNormalOfWall();



private:
    Vec3f pointA;
    Vec3f pointB;
    Vec3f IndoorDir;

};

#endif // WALL_H
