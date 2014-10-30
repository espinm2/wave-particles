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

Vec3f Wall::getNormalOfWall() {
    // There are two normals
    // We want to return the normal whose angle is closest
    // To the interior angle

    assert(pointA.z() == pointB.z());

    double dx = pointB.x() - pointA.x();
    double dy = pointB.y() - pointA.y();

    Vec3f opt1(-dy,dx,0); opt1.Normalize();
    Vec3f opt2(dy,-dx,0); opt2.Normalize();

    if( fabs(IndoorDir.AngleBetween(opt1)) < fabs(IndoorDir.AngleBetween(opt2)) ){
       return opt1;
    }else{
        return opt2;
    }
}



