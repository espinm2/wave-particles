#ifndef PARTICLE_H
#define PARTICLE_H

#include <iostream>

class Particle
{
    public:

      // constructor
      Particle(const glm::vec3 & pos, glm::vec3 & dir, double a){
          // Note orginal posititon is not created
          position = pos;
          direction = dir;
          ampage   = a;
      }

      // ACCESSORS
      const glm::vec3 & getOrginalPos() const{ return original_position; }
      const glm::vec3 & getPos() const { return position; }
      const glm::vec3 & getVel() const { return direction; }
      double getAmp() const { return ampage; }
      void print(){
        std::cout << "=================================" << std::endl;
        std::cout << "Particle " << this << std::endl;
        std::cout << "Pos (" << position.x  << ", " << position.y  << ", " <<  position.z  << ")"<< std::endl;
        std::cout << "Dir (" << direction.x << ", " << direction.y << ", " <<  direction.z << ")"<< std::endl;
        std::cout << "Orp (" << original_position.x << ", " << original_position.y << ", " <<  original_position.z << ")"<< std::endl;
        std::cout << "Amp " << ampage << std::endl;
      }

      // MODIFIERS
      void setOrginalPos(const glm::vec3 & pos){ original_position = pos; }
      void setPos(const glm::vec3 & pos){ position = pos; }
      void setVel(const glm::vec3 & vel){ direction = vel; }
      void setAmp(double a) { ampage = a; }

    private:

      // REPRESENTATION
      glm::vec3 original_position;
      glm::vec3 position;
      glm::vec3 direction;
      double ampage;
};
#endif // PARTICLE_H
