#ifndef PARTICLE_H
#define PARTICLE_H

#include <cassert>
#include <iostream>
#include <cmath>
#include <string>

class Particle
{
    public:

      // defualt constructor
      Particle(){
        position     = glm::vec3(0,0,0);
        oldPosition  = glm::vec3(0,0,0);
        direction    = glm::vec3(0,0,0);
        ampage       = 0;
        splits       = 0;
      }

      // constructor
      Particle(const glm::vec3 & pos, const glm::vec3 & old, const glm::vec3 & dir, const glm::vec3 c, double a, int s){
          position    = pos;
          oldPosition = old;
          direction   = dir;
          ampage      = a;
          splits      = s;
          center      = c;
      }

      // Accessors
      const   glm::vec3 & getPos()    const { return position; }
      const   glm::vec3 & getOldPos() const { return position;}
      const   glm::vec3 & getCenter() const { return center; }
      const   glm::vec3 & getDir()    const { return direction; }
      double  getAmp()                const { return ampage; }
      int     getSplit()              const { return splits; }


      // Modifiers
      void setPos     (const glm::vec3 & pos) { position = pos; }
      void setOldPos  (const glm::vec3 & pos) { oldPosition = pos; }
      void setCenter  (const glm::vec3 & pos) { center = pos; }
      void setDir     (const glm::vec3 & dir) { direction = dir; }
      void setAmp     (double a)              { ampage = a; }
      void setSplit   (int s)                 { splits = s; }

      // Debugging Functions
      friend std::ostream& operator<<(std::ostream &, const Particle &);

private:

      // Rep
      glm::vec3 position;
      glm::vec3 oldPosition;
      glm::vec3 center;
      glm::vec3 direction;
      double    ampage;
      int       splits;
};


// Printing particle inline
inline std::ostream & operator<<(std::ostream & leftOp, const Particle & rightOp){

    leftOp << "Loc " << &rightOp << " ";
    leftOp << "pos(" << rightOp.position.x  << ", " << rightOp.position.y  << ", " <<  rightOp.position.z  << ") ";
    leftOp << "old(" << rightOp.oldPosition.x  << ", " << rightOp.oldPosition.y  << ", " <<  rightOp.oldPosition.z  << ") ";
    leftOp << "dir(" << rightOp.direction.x << ", " << rightOp.direction.y << ", " <<  rightOp.direction.z << ") ";
    leftOp << "cen(" << rightOp.center.x    << ", " << rightOp.center.y    << ", " <<  rightOp.center.z    << ") ";
    leftOp << "amp " << rightOp.ampage      << " ";
    leftOp << "spl " << rightOp.splits;
    return leftOp;
}

#endif // PARTICLE_H
