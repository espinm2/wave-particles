#include "wall.h"


/*
Wall::Wall(const Vec3f &a, const Vec3f &b, const Vec3f &dir){

    pointA = a;
    pointB = b;
    IndoorDir = dir;

}
*/

Wall::Wall(Vec3f a, Vec3f b,  Vec3f dir){

    pointA = a;
    pointB = b;
    IndoorDir = dir;

}

// Setter and getter functions
Vec3f Wall::getA() const {
    return pointA;
}

void Wall::setA(const Vec3f &value) {
    pointA = value;
}

Vec3f Wall::getB() const {
    return pointB;
}

void Wall::setB(const Vec3f &value) {
    pointB = value;
}

Vec3f Wall::getIndoorDir() const {
    return IndoorDir;
}

void Wall::setIndoorDir(const Vec3f &value) {
    IndoorDir = value;
}



