#include <cassert>
#include <cmath>
#include <math.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>// glm::normalize, glm::dot, glm::reflect

#include "particlesystem.h"
#include "MersenneTwister.h"
#include "argparser.h"
#include "grid.h"

typedef std::vector <Particle *>::iterator PartIter;
int number_overlapped = 0;
const bool DEBUG_TOGGLE = false;


// ====================================================================
// Constructor
ParticleSystem::ParticleSystem(ArgParser *a) : args(a){

  Grid g(100,100,10); // Fix me make me contant
  this->particleGrid = g;
  timestep        = .2;
  isBounded       = true;
  initAmps        = 1000;
  minAmps         = 10;
  velocity        = 1;
  particleRadius  = 1;
  clusterRadius   = .1;
  initClusterSize = 4;
}

// ====================================================================
// Update Simulation
void ParticleSystem::update(){

  /*
   * Input : None
   * Output: This function will update our particle simuations
   * Asumpt: There are particles to move
   * SideEf: Updates postition of particles/ removes particles
   */

  // Debug Stuff ////////////////
  int newly_made_particles = 0;//
  int number_splits        = 0;//
  int number_deleted       = 0;//
  number_overlapped = 0;       //
  bool splitReached = false;   //
  // ////////////////////////////

  // Hold new particles from split
  std::vector<Particle *> newParticles;

  // Marked for removal mask, 1 == delete, 0 == keep
  std::vector<int>deleteMask (particleRings.size(), 0);
  unsigned int maskIndex = 0;

  for(PartIter iter = particleRings.begin(); iter != particleRings.end();){

     // This current Particle
     Particle * curPart = (*iter);
     curPart->setOldPos(curPart->getPos());


    // Are we below a threhold just kill and move to another
    if(curPart->getAmp() < minAmps){

      // Kill this partcile and move to next
      deleteMask[maskIndex++] = 1;
      iter++;
      number_deleted++;
      continue;

    }

    // Particles are beyond a threshold init a split
    if(outOfRange(curPart)){

        // Debug ///////
        number_splits++;
        if( curPart->getSplit() == 1 ){ splitReached = true ; }

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
        newly_made_particles+=3;                //
        //  //////////////////////////////////////

        // Debug Test ///////////////////
        assert( &(*curPart) != &(*c) );//
        number_deleted++;              //
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
  for( unsigned int i = 0 ; i < particleRings.size(); i++){
      // Keep if 0, else delete
      if(deleteMask[i] == 1){

          // Remove from grid strucutre
          Vec3f pos = particleRings[i]->getOldPos();
          Cell * c = particleGrid.getCellCoordinates(pos.x(), pos.y());
          bool removed = c->removeParticle(particleRings[i]);
          if(!removed){
              particleGrid.bruteSearch(particleRings[i]);
              assert(false);
          }


          if(!newParticles.empty()){

              // Put in new particle to fill gap
              delete particleRings[i];
              particleRings[i] = newParticles.back();
              newParticles.pop_back();

          }else{

              // there is stuff to push off
              if(i != particleRings.size()-1){

                // Pop off back of vector to fill the gap
                delete particleRings[i];
                particleRings[i] = particleRings.back();
                particleRings.pop_back();

              }else{
                // Just delete the last element, nothing need be poped
                delete particleRings[i];
                particleRings.pop_back();

              }
          }
      }
  }


  // Add into the main vector those new particles yet added
  for( unsigned int i = 0; i < newParticles.size(); i++)
      particleRings.push_back(newParticles[i]);

  if(DEBUG_TOGGLE){
  
    // Print Stats
    if(splitReached){
        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
        std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
    }

      std::cout << "==========================" << std::endl;
      std::cout << "Total Number of Particles " << particleRings.size() << std::endl;
      std::cout << "Total Number of Splits "    << number_splits << std::endl;
      std::cout << "New Made Particles "        << newly_made_particles << std::endl;
      std::cout << "Number of Deleted "         << number_deleted << std::endl;
      std::cout << "Number of Overlapps "       << number_overlapped << std::endl;

  }


  // Recreate the VBOs
  setupVBOs();

}


// ====================================================================
// Setup Functions
void ParticleSystem::setupVBOs() {
  HandleGLError("enter setupVBOs");
  setupPoints();
  HandleGLError("leaving setupVBOs");
}

void ParticleSystem::setupPoints() {

  HandleGLError("enter setupPoints");

  // allocate space for the data

  VertexPosColor* points = new VertexPosColor[particleRings.size()];
  MTRand randGen;

  unsigned int countSplit2 = 0;
  for(unsigned int i = 0; i < particleRings.size(); i++){

     Particle * curPart = particleRings[i];


     // Getting posititons
     Vec3f pos_2d = curPart->getPos();
     //glm::vec4 pos_3d(pos_2d.x(), pos_2d.y(), randGen.randInt(1), 1); //locked for 2d case
     glm::vec4 pos_3d(pos_2d.x(), pos_2d.y(), 0, 1); //locked for 2d case

     glm::vec4 color;
     // even
     int split = curPart->getSplit();

     if(split%3 == 0){
         // color = glm::vec4((i/(double)particleRings.size()),0,0,1);//red
         color = glm::vec4(1,0,0,0.5);//red
     }else if(split%3 == 1){
         // color = glm::vec4(0,0,(i/(double)particleRings.size()),1);//blue
         color = glm::vec4(0,0,1,0.5);//blue
     } else if(split%3 == 2){
         // color = glm::vec4(0,(i/(double)particleRings.size()),0,1);//green
         color = glm::vec4(0,1,0,0.5);//green
         countSplit2++;
     }else{
         color = glm::vec4(1,0,1,0.5);//black
     }


     // Adding to VBO
     points[i] = VertexPosColor(pos_3d,color);

   }


  // create a pointer for the VBO
  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);

  // Where we are storing these points
  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER,VboId);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * particleRings.size(), points,GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)sizeof(glm::vec4));

  // increase the size of the points slightly
  glPointSize(5.0);

  delete [] points;
  HandleGLError("leaving setupPoints");
}

// ====================================================================
// Render Functions
void ParticleSystem::drawVBOs(GLuint MatrixID,const glm::mat4 &m) {
  HandleGLError("enter drawVBOs");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &m[0][0]);
  drawPoints();
  HandleGLError("leaving drawVBOs");
}

void ParticleSystem::drawPoints() const {
  HandleGLError("enter drawPoints");
  // glDrawArrays draws the points
  glDrawArrays(
              GL_POINTS,    // The primitive you want to draw it as
              0,            // The start offset
              particleRings.size());  // The number of points

  HandleGLError("leaving drawPoints");
}

void ParticleSystem::cleanupVBOs() {
  HandleGLError("enter cleanupVBOs");
  cleanupPoints();
  HandleGLError("leaving cleanupVBOs");
}

void ParticleSystem::cleanupPoints() {
  glDeleteBuffers(1, &VaoId);
  glDeleteBuffers(1, &VboId);
}


// ====================================================================
// Utility Functions

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

  long double radianAngle   = angleBetween(axis,dir,norm);
  double distanceFromCenter = center.Distance3f(oldPos);


  // Get the offset angle (Math for this angle might be fuzzy)
  double offsetAngle = (2 * M_PI) / (initClusterSize * pow(3,curPart->getSplit() + 1));

  // Left Particle
  long double radianAngleLeft = radianAngle + offsetAngle;
  Vec3f newPosLeft = getPosCircle(distanceFromCenter,radianAngleLeft,center);
  Vec3f dirLeft(newPosLeft-center);
  dirLeft.Normalize();

  c->setPos    (newPosLeft);
  c->setOldPos (newPosLeft);
  c->setDir    (dirLeft);
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
  a->setDir    (dirRight);
  a->setCenter (center);
  a->setAmp    (curPart->getAmp()/3.0);
  a->setSplit  (curPart->getSplit()+1);

  // Center particle == Orginal curPart
  b->setPos    (oldPos);
  b->setOldPos (oldPos);
  b->setDir    (dir);
  b->setCenter (center);
  b->setAmp    (curPart->getAmp()/3.0);
  b->setSplit  (curPart->getSplit()+1);

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

  double radianAngle = angleBetween(axis,dir,norm);
  double vx          = velocity * cos(radianAngle);
  double vy          = velocity * sin(radianAngle);

  Vec3f newPos( oldPos.x() + (vx * timestep) , oldPos.y() + (vy * timestep), 0 );

  // Update position
  curPart->setPos( newPos ); 

  // Change direction of particle
  if(isBounded){

    double distanceFromCenter = newPos.Distance3f(curPart->getCenter());

    // hit a vertical wall
    if(newPos.x() < 0 || 100 < newPos.x()){
      Vec3f newCenter = getPosCircle(distanceFromCenter,-1*radianAngle,newPos);
      Vec3f newDir(-1*dir.x(), dir.y(),0);
      curPart->setDir(newDir);
      curPart->setCenter(newCenter);
    }

    // Hit a horizontal wall
    if(newPos.y()< 0 || 100 < newPos.y()){
      Vec3f newCenter = getPosCircle(distanceFromCenter,(-1*radianAngle)+M_PI,newPos);
      Vec3f newDir(dir.x(), -1*dir.y(), 0);
      curPart->setDir(newDir);
      curPart->setCenter(newCenter);
    }
  }
}

bool ParticleSystem::outOfRange(Particle * p){
 /* Input : A single particle
  * Output: True if you don't have buddy, false if you do, uses old pos
  * Asumpt: Valid iterator
  * SideEf: Delete from current particle from main vec and heap
  */

  Vec3f pos = p->getOldPos();

  double threshold = .5 * particleRadius; // TODO change to real value

  double nearestDistance = 100000;

  // Debugging
  Particle * nearestPart;
  int nearestIndex = -1;


  // Returns closest particle
  for(unsigned int i = 0; i < particleRings.size(); i++ ){

      // Don't count myself as a buddy
      if(particleRings[i] == p)
          continue;

      double dist = particleRings[i]->getOldPos().Distance3f(pos);

      if(nearestDistance > dist){
          nearestDistance = dist;
          nearestPart = particleRings[i];
          nearestIndex = i;
      }

   }


  // using a epsion
  if(fabs(nearestDistance-threshold) <= .0001 || nearestDistance > threshold){
      return true;
  }else{
      if(nearestDistance < .000001){ number_overlapped++; }
      return false;
  }
}

double ParticleSystem::angleBetween(Vec3f a, Vec3f b, Vec3f norm){
    /*
     *  Input : Two vectors and the normal between them
     *  Output: The angle between both vectors in the range of [-PI , PI]
     */
    double angle = acos( a.Dot3(b) / ( a.Length() * b.Length() ) );
    Vec3f c;
    Vec3f::Cross3(c,norm,a);
    if(c.Dot3(b) < 0)
        angle = -angle;

    return angle;
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
    double radianAngle = ( 2 * M_PI ) / initClusterSize;          

    // For each particle I will make
    for(unsigned int i = 0; i < initClusterSize; i++){

      // Get changes in postiton
      double dx = (cos(radianAngle * i) * clusterRadius); 
      double dy = (sin(radianAngle * i) * clusterRadius); 

      Vec3f pos_2d(x + dx, y + dy, 0.0);

      // Set velolcity and normalize
      Vec3f dir_2d = pos_2d - center;
      dir_2d.Normalize();

      // Create this new particle
      Vec3f pos_3d(pos_2d);
      Vec3f dir_3d(dir_2d.x(), dir_2d.y(), 0);

      // Create particle on heap
      Particle * newPart = 
        new Particle(
          pos_3d,
          pos_3d,
          dir_3d, 
          center,
          initAmps,
          0);

      // Add to collection
      particleRings.push_back(newPart);
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
