#ifndef CELL_H
#define CELL_H

// Import particles, which I will hold
#include "particle.h"
#include <vector>


class Cell{
  public:
      // Implicit default constructor
      Cell(){}
         
      // Commonly used accessors
      const bool isEmpty() const{ return particles.size() == 0; }
      const unsigned int size()const { return particles.size(); }
      const std::vector< Particle *> getParticles() const { return particles;}

      // Commonly used setters
      void push_back( Particle * a){ particles.push_back(a); }

      // Modifiers
      bool removeParticle( Particle * partPtr){

          if(isEmpty())
              return false;

          for( int i = 0; i < particles.size(); i++ ){
              if(particles[i]  == partPtr){

                  particles[i] = particles.back();
                  particles.pop_back();
                  return true;
              }
          }

          return false;
      }



  private:
      std::vector< Particle *> particles;
      
};

#endif // CELL_H
