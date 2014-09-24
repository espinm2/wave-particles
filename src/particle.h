#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>

class Particle
{
    public:

      // constructor
      Particle(const glm::vec3 & pos, glm::vec3 & dir, double a, int s){
          // Note orginal posititon is not created
          position  = pos;
          direction = dir;
          ampage    = a;
          splits    = s;
      }

      // ACCESSORS
      const glm::vec3 & getCenter() const{ return center; }
      const glm::vec3 & getPos() const { return position; }
      const glm::vec3 & getDir() const { return direction; }
      double getAmp() const { return ampage; }
      int getSplit() const { return splits; }
      void print(){
        std::cout << "=================================" << std::endl;
        std::cout << "Particle " << this << std::endl;
        std::cout << "Pos (" << position.x  << ", " << position.y  << ", " <<  position.z  << ")"<< std::endl;
        std::cout << "Dir (" << direction.x << ", " << direction.y << ", " <<  direction.z << ")"<< std::endl;
        std::cout << "Cen (" << center.x << ", " << center.y << ", " <<  center.z << ")"<< std::endl;
        std::cout << "Amp " << ampage << std::endl;
      }

      // MODIFIERS
      void setCenter(const glm::vec3 & pos){ center = pos; }
      void setPos(const glm::vec3 & pos){ position = pos; }
      void setDir(const glm::vec3 & dir){ direction = dir; }
      void setAmp(double a) { ampage = a; }
      void setSplit(int s) { splits = s; }


private:

      // REPRESENTATION
      glm::vec3 center;
      glm::vec3 position;
      glm::vec3 direction;
      double ampage;
      int splits;
};
#endif // PARTICLE_H

