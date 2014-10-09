#include <cassert>
#include <cmath>
#include <math.h>

#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>// glm::normalize, glm::dot, glm::reflect

#include "particlesystem.h"
#include "argparser.h"

typedef std::vector <Particle *>::iterator PartIter;


// ====================================================================
// Constructor
ParticleSystem::ParticleSystem(ArgParser *a) : args(a){
  timestep        = .001;
  isBounded       = false;
  initAmps        = 1000;
  minAmps         = 10;
  velocity        = 1;
  particleRadius  = 2;
  clusterRadius   = .1;
  clusterSize     = 4;
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
      continue;

    }

    // Particles are beyond a threshold init a split
    if(outOfRange(curPart)){

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

        // Update postiton and move to next particle
        moveParticle(curPart);
        deleteMask[maskIndex++] = 0;
        iter++;
    }//ifelse

  } //forloop


  std::vector<Particle *> updatedRings;

  for( unsigned int i = 0 ; i < particleRings.size(); i++){

      // Keep if 0, else delete
      if(deleteMask[i] == 0)
          updatedRings.push_back(particleRings[i]);
      else
          delete particleRings[i];
  }


  // Add into the main vector those new particles yet added
  for( unsigned int i = 0; i < newParticles.size(); i++)
      updatedRings.push_back(newParticles[i]);


  // Update particleRings
  this->particleRings = updatedRings;

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

  for(unsigned int i = 0; i < particleRings.size(); i++){

     Particle * curPart = particleRings[i];


     // Getting posititons
     glm::vec3 pos_2d = curPart->getPos();
     glm::vec4 pos_3d(pos_2d.x, pos_2d.y, 0,1); //locked for 2d case

     glm::vec4 color;
     // even
     int split = curPart->getSplit();

     if(split%3 == 0){
         color = glm::vec4((i/(double)particleRings.size()),0,0,1);//red
     }else if(split%3 == 1){
         color = glm::vec4(0,0,(i/(double)particleRings.size()),1);//blue
     } else if(split%3 == 2){
         color = glm::vec4(0,(i/(double)particleRings.size()),0,1);//green
     }else{
         color = glm::vec4(0,0,0,1);//black
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
  glm::vec3 oldPos  = curPart->getOldPos();
  glm::vec3 dir     = curPart->getDir();
  glm::vec3 center  = curPart->getCenter();

  glm::vec3 axis    = glm::vec3(1,0,0);
  glm::vec3 norm    = glm::vec3(0,0,1);

  long double radianAngle   = angleBetween(axis,dir,norm);
  float distanceFromCenter = glm::distance(center, oldPos);


  // Get the offset angle (Math for this angle might be fuzzy)
  long double angleBetweenParticles = ( 2 * M_PI ) / clusterSize;
  long double offsetAngle = angleBetweenParticles / (3.0 * (curPart->getSplit() + 1));

  // Left Particle
  long double radianAngleLeft = radianAngle + offsetAngle;
  glm::vec3 newPosLeft = getPosCircle(distanceFromCenter,radianAngleLeft,center);
  glm::vec3 dirLeft(newPosLeft-center);
  dirLeft = glm::normalize(dirLeft);

  c->setPos    (newPosLeft);
  c->setOldPos (newPosLeft);
  c->setDir    (dirLeft);
  c->setCenter (center);
  c->setAmp    (curPart->getAmp()/3.0);
  c->setSplit  (curPart->getSplit()+1);

  // Right Particle
  long double radianAngleRight = radianAngle - offsetAngle;
  glm::vec3 newPosRight = getPosCircle(distanceFromCenter,radianAngleRight,center);
  glm::vec3 dirRight(newPosRight-center);
  dirRight = glm::normalize(dirRight);

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
  glm::vec3 oldPos = curPart->getOldPos();
  glm::vec3 dir    = curPart->getDir();

  glm::vec3 axis   = glm::vec3(1,0,0);
  glm::vec3 norm   = glm::vec3(0,0,1);

  double radianAngle = angleBetween(axis,dir,norm);
  double vx          = velocity * cos(radianAngle);
  double vy          = velocity * sin(radianAngle);

  glm::vec3 newPos( oldPos.x + (vx * timestep) , oldPos.y + (vy * timestep), 0 );

  // Update position
  curPart->setPos( newPos ); 

  // Change direction of particle
  if(isBounded){

    float distanceFromCenter = glm::distance(newPos,curPart->getCenter());

    // hit a vertical wall
    if(newPos.x < 0 || 1 < newPos.x ){
      glm::vec3 newCenter = getPosCircle(distanceFromCenter,-1*radianAngle,newPos);
      glm::vec3 newDir(-1*dir.x, dir.y,0);
      curPart->setDir(newDir);
      curPart->setCenter(newCenter);
    }

    // Hit a horizontal wall
    if(newPos.y < 0 || 1 < newPos.y ){
      glm::vec3 newCenter = getPosCircle(distanceFromCenter,(-1*radianAngle)+M_PI,newPos);
      glm::vec3 newDir(dir.x, -1*dir.y, 0);
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

  glm::vec3 pos = p->getOldPos();

  double threshold = .5 * particleRadius; // TODO change to real value
  float nearestDistance = 100000;
  Particle * nearestPart;

  // Returns closest particle
  for(unsigned int i = 0; i < particleRings.size(); i++ ){

      // Don't count myself as a buddy
      if(particleRings[i] == p)
          continue;

      float dist = glm::distance(particleRings[i]->getOldPos(), pos);

      if(nearestDistance > dist){
          nearestDistance = dist;
          nearestPart = particleRings[i];
      }

   }

  // using a epsion
  if(fabs(nearestDistance-threshold) <= .01 || nearestDistance > threshold){
      return true;
  }else{
      return false;
  }
}

PartIter ParticleSystem::removeParticle(PartIter iter){
  /* Input : Iterator to a particle in the main vector
   * Output: The iterator the particle after the one given
   * Asumpt: Valid iterator
   * SideEf: Delete from current particle from main vec and heap
   */

    Particle *p       = *iter;
    PartIter  newIter = particleRings.erase(iter);
    delete    p;
    return newIter;

}

double ParticleSystem::angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 norm){
    /*
     *  Input : Two vectors and the normal between them
     *  Output: The angle between both vectors in the range of [-PI , PI]
     */
    double angle = acos( glm::dot(a,b) / (glm::length(a) * glm::length(b)) );
    glm::vec3 c = glm::cross(norm,a);
    if(glm::dot(c,b) < 0)
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
    glm::vec2 center(x,y);

    // Create a cluster of wave particles when you click
    double radianAngle = ( 2 * M_PI ) / clusterSize;          

    // For each particle I will make
    for(unsigned int i = 0; i < clusterSize; i++){

      // Get changes in postiton
      double dx = (cos(radianAngle * i) * clusterRadius); 
      double dy = (sin(radianAngle * i) * clusterRadius); 

      glm::vec2 pos_2d(x + dx, y + dy);

      // Set velolcity and normalize
      glm::vec2 dir_2d = glm::normalize(pos_2d - center);

      // Create this new particle
      glm::vec3 pos_3d(pos_2d.x, pos_2d.y, 0.0f);
      glm::vec3 old_3d(0,0,0);
      glm::vec3 dir_3d(dir_2d.x, dir_2d.y, 0.0f);
      glm::vec3 cen_3d(center,0);

      // Create particle on heap
      Particle * newPart = 
        new Particle(
          pos_3d,
          pos_3d,
          dir_3d, 
          cen_3d,
          initAmps,
          0);

      // Add to collection
      particleRings.push_back(newPart);

    }

}

glm::vec3 ParticleSystem::getPosCircle(double radius, double radAngle, glm::vec3 center){
  /*
   * Input : center of a circle, the angle relative to X axis, and radius
   * Output: Point on a circle with those attrabutes
   * Asumpt: radAngle is radians and like a unit circle, 2D
   * SideEf: None
   */
    return glm::vec3(center.x + radius * cos(radAngle), center.y + radius * sin(radAngle),0);
}
