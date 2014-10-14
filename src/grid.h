#ifndef GRID_H
#define GRID_H

#include "cell.h"


class Grid
{

  public:

    // Given the world size I am working with + how much id like to 
    // split the world up make an even cell division for this world
    Grid(unsigned int world_width, unsigned int world_hight, 
        unsigned int divisions);
    
    // accessors
    Cell * getCell(unsigned int i, unsigned int j);

  private:
    std::vector <Cell *> grid_vector;
    unsigned int divisions;


};





#endif // GRID_H
