#include "grid.h"

#include "cell.h"

#include <cassert>
#include <math.h>
/*
#include "particle.h"
 * This is the implementations for all the methods and util methods
 * used for our grid object we use to store particles in.
 */

// [x] Create constructor
// [x] Create getter
// [ ] Test these


// constructor
Grid::Grid(unsigned int world_width, unsigned int world_hight, 
        unsigned int divisions){

  this->divisions = divisions;

  // init a cell on heap and keep a pointer to it
  for( int i = 0; i < pow(divisions,2); i++ ){
    Cell * c = new Cell;
    grid_vector.push_back(c);
  }

}

// I will returns a pointer to a cell here
Cell * Grid::getCell(unsigned int i, unsigned int j){

  // Errors in getting the index
  assert(0 <= i && i < divisions);
  assert(0 <= j && j < divisions);

  unsigned int index = (divisions * i) + j;
  return grid_vector[index];
  
}








