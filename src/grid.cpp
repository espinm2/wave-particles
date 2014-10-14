#include "grid.h"

#include "cell.h"
#include "vectors.h"

#include <cassert>
#include <math.h>
/*
#include "particle.h"
 * This is the implementations for all the methods and util methods
 * used for our grid object we use to store particles in.
 */

// [x] Create constructor
// [x] Create getter
// [x] Test these


// Default constrcutor
Grid::Grid(){
    divisions  = 0;
    width      = 0;
    height     = 0;
}

// constructor
Grid::Grid(unsigned int world_width, unsigned int world_height,
        unsigned int divisions){

  this->divisions = divisions;
  this->width = world_width;
  this->height = world_height;

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


// This function will return a pointer to cell given
// world coordinates.
Cell * Grid::getCellCoordinates(double x, double y){

  unsigned int x_index = x / divisions;
  unsigned int y_index = y / divisions;

  // Out of range checks, that you
  if(x_index >= divisions){
      x_index = divisions - 1;
      std::cout << "right wall pass";
  }

  if(x_index < 0){
      x_index = 0;
      std::cout << "left wall pass";
  }

  if(y_index >= divisions){
      y_index = divisions - 1;
      std::cout << "top wall pass";
  }

  if(y_index < 0){
      y_index = 0;
      std::cout << "bottm wall passed";
  }

  Cell * cell = getCell(x_index, y_index);

  std::cout << "Inputs "  <<  x << " " << y << std::endl;
  std::cout << "Get Cell Coordinates " << x_index << " " << y_index << std::endl;

  return cell;
}

// Put particle inside of the cell
void Grid::putParticleInGrid(Particle * c){

  // Put particle inside the correct cell it in contained in
  Cell * cell = getCellCoordinates( c->getPos().x(), c->getPos().y() );
  cell->push_back(c);
}

Cell * Grid::getOldParticleCell(Particle * c){
    return getCellCoordinates( c->getOldPos().x(), c->getOldPos().y() );
}

Cell * Grid::getParticleCell(Particle * c){
    return getCellCoordinates( c->getOldPos().x(), c->getOldPos().y() );
}

void Grid::bruteSearch(Particle *c){

    for(int i = 0 ; i < grid_vector.size(); i ++){

        for(int j = 0; j < grid_vector[i]->size(); j++){

            Particle  *  curPart =  grid_vector[i]->getParticles()[j];

            if(curPart == c){
                int x_index = i / divisions;
                int y_index = i % divisions;
                std::cout << "Found at " << x_index << " " <<  y_index << std::endl;
            }
        }
    }
}



