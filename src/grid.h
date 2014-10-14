#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include "particle.h"


class Grid
{

  public:

    // Default constructor
    Grid();

    // Given the world size I am working with + how much id like to 
    // split the world up make an even cell division for this world
    Grid(unsigned int world_width, unsigned int world_hight, 
        unsigned int divisions);
    
    // accessors
    Cell * getCell(unsigned int i, unsigned int j);
    Cell * getCellCoordinates(double x, double y);
    Cell * getOldParticleCell(Particle * c);
    Cell * getParticleCell(Particle * c);
    void bruteSearch(Particle * c);

    // particle modifiers
    void putParticleInGrid(Particle * c);



  private:
    std::vector <Cell *> grid_vector;
    unsigned int divisions;
    unsigned int width;
    unsigned int height;

};





#endif // GRID_H
