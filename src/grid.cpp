#include "grid.h"


Grid::Grid(){
    divisions  = 0;
    width      = 0;
    height     = 0;
}

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


/*  // Failing when used
    Grid::~Grid(){
        for ( int i = 0; i < grid_vector.size(); i++){
            delete grid_vector[i];
        }
    }
*/

Cell * Grid::getCell(unsigned int i, unsigned int j){

  // Errors in getting the index
  assert(0 <= i && i < divisions);
  assert(0 <= j && j < divisions);

  unsigned int index = (divisions * i) + j;
  return grid_vector[index];
  
}


Cell * Grid::getCellCoordinates(double x, double y){

  unsigned int x_index = x / divisions;
  unsigned int y_index = y / divisions;

  // Out of range checks, that you
  if(x_index >= divisions){
      x_index = divisions - 1;
  }

  if(x_index < 0){
      x_index = 0;
  }

  if(y_index >= divisions){
      y_index = divisions - 1;
  }

  if(y_index < 0){
      y_index = 0;
  }

  Cell * cell = getCell(x_index, y_index);

  if((x <= 0 || x >= 100) && (y <= 0 || y >= 100)){
    std::cout << "Inputs "  <<  x << " " << y << std::endl;
    std::cout << "Get Cell Coordinates " << x_index << " " << y_index << std::endl;
  }

  return cell;
}

void Grid::putParticleInGrid(Particle * p){

  // Put particle inside the correct cell it in contained in
  Cell * cell = getCellCoordinates( p->getPos().x(), p->getPos().y() );
  cell->push_back(p);

}

Cell * Grid::getOldParticleCell(Particle * p){
    return getCellCoordinates( p->getOldPos().x(), p->getOldPos().y() );
}

Cell * Grid::getParticleCell(Particle * p){
    return getCellCoordinates( p->getOldPos().x(), p->getOldPos().y() );
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


std::vector<Cell *> Grid::getParticleCellAdj(Particle * c){

  // Where there result is
  std::vector<Cell *> res;

  // Get the particles main location mapped
  unsigned int x_index = c->getOldPos().x() / divisions;
  unsigned int y_index = c->getOldPos().y() / divisions;

  if(x_index >= divisions){
      x_index = divisions - 1;
  }

  if(x_index < 0){
      x_index = 0;
  }

  if(y_index >= divisions){
      y_index = divisions - 1;
  }

  if(y_index < 0){
      y_index = 0;
  }

  // Push all back that I can

  for( int x = x_index - 1; x <= y_index + 1; x++){

    for( int y = y_index - 1; y <= y_index + 1; y++){

      if(0 <= x && x < divisions && 0 <= y && y < divisions){
          res.push_back(getCell(x,y));
      }
    }
  }

  return res;

}

