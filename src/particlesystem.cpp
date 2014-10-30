#include "particlesystem.h"

typedef std::vector <Particle *>::iterator PartIter;

ParticleSystem::ParticleSystem(ArgParser *a) : args(a){

  // Make sure we don't get to small, that it effects out splits
  assert((args->worldRange / args->gridDivisions) > splitDistance());

  // Create a grid to use
  this->particleGrid =
            Grid(args->worldRange,
                 args->worldRange,
                 args->gridDivisions);

  // Load in walls
  // loadWallsFromFile("");

}

ParticleSystem::~ParticleSystem(){
    for(unsigned int i = 0; i < particleVec.size(); i++)
        delete particleVec[i];
    for(unsigned int i = 0; i < wallVec.size(); i++)
        delete wallVec[i];
}

void ParticleSystem::update(){

  /*
   * Input : None
   * Output: This function will update our particle simuations
   * Asumpt: There are particles to move
   * SideEf: Updates postition of particles/ removes particles
   */


  // Hold new particles from split
  std::vector<Particle *> newParticles;

  // Marked for removal mask, 1 == delete, 0 == keep
  std::vector<int>deleteMask (particleVec.size(), 0);
  unsigned int maskIndex = 0;

  for(PartIter iter = particleVec.begin(); iter != particleVec.end();){

     // This current Particle
     Particle * curPart = (*iter);
     curPart->setOldPos(curPart->getPos());


    // Are we below a threhold just kill and move to another
    if(curPart->getAmp() < args->minAmps){

      // Kill this partcile and move to next
      deleteMask[maskIndex++] = 1;
      iter++;
      continue;

    }

    // Particles are beyond a threshold init a split
    if(outOfRangeGrid(curPart)){

        // Debug ///////
        // if( curPart->getSplit() == 1 ){ splitReached= true ; }

        // Create 3 new particles and assign them
        Particle * a = new Particle;
        Particle * b = new Particle;
        Particle * c = new Particle;

        splitParticle(curPart,a,b,c);

        // Move them a timestep
        moveParticle(a);
        moveParticle(b);
        moveParticle(c);

        // Push them into newParticles list for l8 addition
        newParticles.push_back(a);
        newParticles.push_back(b);
        newParticles.push_back(c);

        // Push them into the grid data structure
        particleGrid.putParticleInGrid(a);
        particleGrid.putParticleInGrid(b);
        particleGrid.putParticleInGrid(c);

        // Debug Test ////////////////////////////
        assert(curPart->getAmp() > a->getAmp());//
        assert(curPart->getAmp() > b->getAmp());//
        assert(curPart->getAmp() > c->getAmp());//
        //  //////////////////////////////////////

        // Debug Test ///////////////////
        assert( &(*curPart) != &(*c) );//
        //  /////////////////////////////

        // Remove and kill the current particle
        deleteMask[maskIndex++] = 1;
        iter++;


    }else{

        // remove from old place in cell
        bool removed = particleGrid.getOldParticleCell(curPart)->removeParticle(curPart);
        assert(removed); // Make sure removed

        // Update postiton and move to next particle
        moveParticle(curPart);

        // Update our grid datastructure
        particleGrid.putParticleInGrid(curPart);

        deleteMask[maskIndex++] = 0;
        iter++;
    }//ifelse

  } //forloop


  // Deletetion step
  for( unsigned int i = 0 ; i < particleVec.size(); i++){
      // Keep if 0, else delete
      if(deleteMask[i] == 1){

          // Remove from grid strucutre
          Vec3f pos = particleVec[i]->getOldPos();
          Cell * c = particleGrid.getCellCoordinates(pos.x(), pos.y());
          bool removed = c->removeParticle(particleVec[i]);

          if(!removed){
              particleGrid.bruteSearch(particleVec[i]);
              assert(false);
          }


          if(!newParticles.empty()){

              // Put in new particle to fill gap
              delete particleVec[i];
              particleVec[i] = newParticles.back();
              newParticles.pop_back();

          }else{

              // there is stuff to push off
              if(i != particleVec.size()-1){

                // Pop off back of vector to fill the gap
                delete particleVec[i];
                particleVec[i] = particleVec.back();
                particleVec.pop_back();

              }else{

                // Just delete the last element, nothing need be poped
                delete particleVec[i];
                particleVec.pop_back();

              }
          }
      }
  }


  // Add into the main vector those new particles yet added
  for( unsigned int i = 0; i < newParticles.size(); i++)
      particleVec.push_back(newParticles[i]);

  // Recreate the VBOs
  this->setupVBOs();

}

void ParticleSystem::createWall()
{

  Wall * wall = new Wall(args->tempWallPosA, args->tempWallPosB, Vec3f(50,50,0));
  wallVec.push_back(wall);
  particleGrid.putWallInGrid(wall);
  args->createWallRequest = false;


}


void ParticleSystem::splitParticle(Particle * curPart, Particle *a, Particle *b, Particle *c){

  /*
   * Input : None
   * Output: This function will assign the attrubutes of 3 particles made after a split
   * Asumpt: Valid particle made and are on the heap and main vec
   * SideEf: changes in a b c and no touching of curPart
   */


  // Required Vectors for Math
  Vec3f oldPos  = curPart->getOldPos();
  Vec3f dir     = curPart->getDir();
  Vec3f center  = curPart->getCenter();

  Vec3f axis(1,0,0);
  Vec3f norm(0,0,1);

  // long double radianAngle   = angleBetweenLimited(axis,dir,norm);
  long double radianAngle   = axis.AngleBetweenLimited(dir);
  double distanceFromCenter = center.Distance3f(oldPos);


  // Get the offset angle (Math for this angle might be fuzzy)
  double offsetAngle = (2 * M_PI) / (args->initClusterSize * pow(3,curPart->getSplit() + 1));

  // Left Particle
  long double radianAngleLeft = radianAngle + offsetAngle;
  Vec3f newPosLeft = getPosCircle(distanceFromCenter,radianAngleLeft,center);
  Vec3f dirLeft(newPosLeft-center);
  dirLeft.Normalize();

  c->setPos    (newPosLeft);
  c->setOldPos (newPosLeft);
  c->setCenter (center);
  c->setAmp    (curPart->getAmp()/3.0);
  c->setSplit  (curPart->getSplit()+1);

  // Right Particle
  long double radianAngleRight = radianAngle - offsetAngle;
  Vec3f newPosRight = getPosCircle(distanceFromCenter,radianAngleRight,center);
  Vec3f dirRight(newPosRight-center);
  dirRight.Normalize();

  a->setPos    (newPosRight);
  a->setOldPos (newPosRight);
  a->setCenter (center);
  a->setAmp    (curPart->getAmp()/3.0);
  a->setSplit  (curPart->getSplit()+1);

  // Center particle == Orginal curPart
  b->setPos    (oldPos);
  b->setOldPos (oldPos);
  b->setCenter (center);
  b->setAmp    (curPart->getAmp()/3.0);
  b->setSplit  (curPart->getSplit()+1);

}

// This is a protoype for testing
void ParticleSystem::loadWallsFromFile( std::string input_file) {
    // TODO make parser for input
    // TODO make sure we call particleGrid.putWallInGrid(..);

    // Test input only works on worldRange = 100
    Wall * w1 = new Wall(Vec3f(0,0,0), Vec3f(100,100,0), Vec3f(50,50,0));
    Wall * w2 = new Wall(Vec3f(80,20,0), Vec3f(80,80,0), Vec3f(50,50,0));

    Wall * w3 = new Wall(Vec3f(20,20,0), Vec3f(80,20,0), Vec3f(50,50,0));
    Wall * w4 = new Wall(Vec3f(20,80,0), Vec3f(80,80,0), Vec3f(50,50,0));


    wallVec.push_back(w1);
    //wallVec.push_back(w2);
    //wallVec.push_back(w3);
    //wallVec.push_back(w4);

    particleGrid.putWallInGrid(w1);
    //particleGrid.putWallInGrid(w2);
    //particleGrid.putWallInGrid(w3);
    //particleGrid.putWallInGrid(w4);


}

void ParticleSystem::moveParticle(Particle *curPart){
  /* Input : A single particle
  *  Output: Nothing
  *  Asumpt: that oldpos has a position
  *  SideEf: Changes the pos and maybe dir of a particle given
  */

  // Change position of particle
  Vec3f oldPos = curPart->getOldPos();
  Vec3f dir    = curPart->getDir();

  Vec3f axis(1,0,0);
  Vec3f norm(0,0,1);

  double radianAngle = axis.AngleBetweenLimited(dir);
  double vx          = args->velocity * cos(radianAngle);
  double vy          = args->velocity * sin(radianAngle);

  Vec3f newPos( oldPos.x() + (vx * args->timestep) , oldPos.y() + (vy * args->timestep), 0 );

  // Update position
  curPart->setPos( newPos ); 

  // Change direction of particle
  if(args->isBounded){

    // TODO implement
    calculateBounces(curPart);

    /* Tagged for destruction */
    double distanceFromCenter = newPos.Distance3f(curPart->getCenter());

    // hit a vertical wall
    if((newPos.x() < 0 && dir.x() < 0 )||( 100 < newPos.x()  && dir.x() > 0)){
      Vec3f newCenter = getPosCircle(distanceFromCenter,-1*radianAngle,newPos);
      Vec3f newDir(-1*dir.x(), dir.y(),0);
      curPart->setCenter(newCenter);
    }

    // Hit a horizontal wall
    if((newPos.y()< 0 && dir.y() < 0)|| (100 < newPos.y() && dir.y() > 0)){
      Vec3f newCenter = getPosCircle(distanceFromCenter,(-1*radianAngle)+M_PI,newPos);
      Vec3f newDir(dir.x(), -1*dir.y(), 0);
      curPart->setCenter(newCenter);
    }
  }
}

void ParticleSystem::calculateBounces(Particle *&p) {
    // Assumes that the particles moved

    Cell * cell = particleGrid.getParticleCell(p);

    // If there are walls in this grid
    if(cell->numWalls() > 0){

        // Get the walls I have to search
        std::vector<Wall *> walls = cell->getWalls();

        for( int i = 0; i < walls.size(); i++){

            // cur wall

            Wall * w = walls[i];
            double xi; double yi;

            // Do I cross the wall?
            if( get_line_intersection( w->getB().x(), w->getB().y(), w->getA().x(), w->getA().y(),
                        p->getPos().x(), p->getPos().y(), p->getOldPos().x(), p->getOldPos().y(), xi,yi))
            {
                Vec3f normal = w->getNormalOfWall();
                normal.Normalize();

                // Am i comming from the inside direction or out?
                if(fabs(p->getDir().AngleBetween(normal)) < 90 ){
                    normal.Negate();
                }

                // Require changing center of particle p
                Vec3f intersect(xi,yi,0);

                Vec3f reflect = p->getDir() - 2 * (p->getDir().Dot3(normal)) * normal;
                reflect.Normalize();

                // Changing direction of particle p
                Vec3f newPos = intersect.Distance3f(p->getPos()) * reflect + intersect;
                p->setPos(newPos);

                reflect.Negate();
                Vec3f circlePos = intersect.Distance3f(p->getCenter())*reflect + intersect;
                p->setCenter(circlePos);




            }

        }

    }

}


bool ParticleSystem::outOfRangeGrid(Particle *p){

    std::vector<Cell *> adjCellVec = particleGrid.getParticleCellAdj(p);

    // Default setup
    Vec3f pos = p->getOldPos();
    double threshold = splitDistance(); // TODO change to real value;
    double nearestDistance = 100000;

    // Returns closest particle
    for(unsigned int c = 0; c < adjCellVec.size(); c++ ){

      std::vector<Particle *> curCellVec = adjCellVec[c]->getParticles();

      for(unsigned int i = 0; i < curCellVec.size(); i++ ){

          // Don't count myself as a buddy
          if(curCellVec[i] == p)
              continue;

          double dist =curCellVec[i]->getOldPos().Distance3f(pos);
          if(nearestDistance > dist)
              nearestDistance = dist;
       }

    }

    // using a epsion
    return (fabs(nearestDistance-threshold) <= .0001 || nearestDistance > threshold);
}


void ParticleSystem::createWave(double x, double y){

    /*
     * Input : None
     * Output: This function make a wave on the XY plane given your input
     * Asumpt: That cluster size is non-zero
     * SideEf: Adds a ring to the memeber varible partileRing
     */

    // Location Pressed / Center of cluster
    Vec3f center(x,y,0);

    // Create a cluster of wave particles when you click
    double radianAngle = ( 2 * M_PI ) / args->initClusterSize;

    // For each particle I will make
    for(unsigned int i = 0; i < args->initClusterSize; i++){

      // Get changes in postiton
      double dx = (cos(radianAngle * i) * args->clusterRadius);
      double dy = (sin(radianAngle * i) * args->clusterRadius);

      Vec3f pos_2d(x + dx, y + dy, 0.0);

      // Set velolcity and normalize
      Vec3f dir_2d = pos_2d - center;
      dir_2d.Normalize();

      // Create this new particle
      Vec3f pos_3d(pos_2d);

      // Create particle on heap
      Particle * newPart = 
        new Particle(
          pos_3d,
          pos_3d,
          center,
          args->initAmps,
          0);

      // Add to collection
      particleVec.push_back(newPart);
      particleGrid.putParticleInGrid(newPart);

    }
}

Vec3f ParticleSystem::getPosCircle(double radius, double radAngle, Vec3f center){
  /*
   * Input : center of a circle, the angle relative to X axis, and radius
   * Output: Point on a circle with those attrabutes
   * Asumpt: radAngle is radians and like a unit circle, 2D
   * SideEf: None
   */
    return Vec3f(center.x() + radius * cos(radAngle), center.y() + radius * sin(radAngle),0);
}
