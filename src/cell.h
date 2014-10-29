#ifndef CELL_H
#define CELL_H

// Import particles, which I will hold
#include "particle.h"
#include "wall.h"
#include <vector>


class Cell{
  public:
      // Implicit default constructor
      Cell(){}
         
      // Numberical Accessors
      const unsigned int numParticles() const { return particleCellVec.size(); }
      const unsigned int numWalls() const { return wallCellVec.size(); }

      // Getters
      const std::vector< Particle *> & getParticles() const { return particleCellVec;}
      const std::vector< Wall *> & getWalls() const { return wallCellVec; }

      // Commonly used setters
      void push_back( Particle * a){ particleCellVec.push_back(a); }
      void push_back( Wall * w ){ wallCellVec.push_back(w); }

      // Modifiers
      bool removeParticle( Particle * partPtr){
          if(particleCellVec.size()==0)
              return false;

          for( unsigned int i = 0; i < particleCellVec.size(); i++ ){
              if(particleCellVec[i]  == partPtr){
                  particleCellVec[i] = particleCellVec.back();
                  particleCellVec.pop_back();
                  return true;
              }
          }
          return false; }

  private:
      std::vector< Particle *> particleCellVec;
      std::vector< Wall *> wallCellVec;
      
};

#endif // CELL_H
