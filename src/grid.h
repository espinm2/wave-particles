#ifndef GRID_H
#define GRID_H

#include "cell.h"
#include "particle.h"
#include "vectors.h"
#include <cassert>
#include <math.h>

class Grid
{

  public:

    // Default constructor
    Grid();

    // Given the world size I am working with + how much id like to 
    // split the world up make an even cell division for this world
    Grid(unsigned int world_width, unsigned int world_hight, 
        unsigned int divisions);

    // Destructor to free cells (failing)
    // ~Grid();
    
    // returns pointer to cell within this grid
    Cell * getCell(unsigned int i, unsigned int j);

    // returns pointer to cell given (x,y) in world space
    Cell * getCellCoordinates(double x, double y);


    // returns pointer to old cel given particle
    Cell * getOldParticleCell(Particle * p);

    // returns pointer to new cell given particle
    Cell * getParticleCell(Particle * p);

    // returns adj cell to input c
    std::vector<Cell *> getParticleCellAdj(Particle * p);

    // searches across all cells and prints cell index
    void bruteSearch(Particle * c);

    // Put a particle inside a specific cell
    void putParticleInGrid(Particle * c);

  private:
    std::vector <Cell *> grid_vector;
    unsigned int divisions;
    unsigned int width;
    unsigned int height;

};

#endif // GRID_H
