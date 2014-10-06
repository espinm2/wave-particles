#ifndef PARTICLE_H
#define PARTICLE_H



#include <cassert>
#include <iostream>

class Particle
{
    public:

      // defualt constructor
      Particle(){
        position  = NULL;
        direction = NULL;
        ampage    = 0;
        splits    = 0;
      }

      // constructor
      Particle(const glm::vec3 & pos, glm::vec3 & dir, double a, int s){
          position  = pos;
          direction = dir;
          ampage    = a;
          splits    = s;
      }

      // Accessors
      const glm::vec3 & getCenter() const{ return center; }
      const glm::vec3 & getPos() const { return position; }
      const glm::vec3 & getDir() const { return direction; }
      double getAmp() const { return ampage; }
      int getSplit() const { return splits; }

      // Debuggers
      void print();

      // Modifiers
      void setCenter(const glm::vec3 & pos){ center = pos; }
      void setPos(const glm::vec3 & pos){ position = pos; }
      void setDir(const glm::vec3 & dir){ direction = dir; }
      void setAmp(double a) { ampage = a; }
      void setSplit(int s) { splits = s; }

      // Sim Methods
      void splitParticle(Particle * a, Particle * b, Particle * c); //TODO


private:

      // Rep
      glm::vec3 center;
      glm::vec3 position;
      glm::vec3 direction;
      double ampage;
      int splits;
};


#endif // PARTICLE_H // maybe me me?


void Particle::print(){
  /*
   * Input : None
   * Output: This function will just print out the repsentation of this particle
   * Asumpt: Valid particle, will fail if you don't set fields to everything
   * SideEf: None
   */

  assert(position != NULL && direction != NULL);

  std::cout << "=================================" << std::endl;
  std::cout << "particle " << this << std::endl;
  std::cout << "pos (" << position.x  << ", " << position.y  << ", " <<  position.z  << ")"<< std::endl;
  std::cout << "dir (" << direction.x << ", " << direction.y << ", " <<  direction.z << ")"<< std::endl;
  std::cout << "cen (" << center.x << ", " << center.y << ", " <<  center.z << ")"<< std::endl;
  std::cout << "amp " << ampage << std::endl;

}

void Particle::splitParticle(Particle * a, Particle * b, Particle * c){
    // TODO implement
}


