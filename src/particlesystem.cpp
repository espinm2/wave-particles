#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>// glm::normalize, glm::dot, glm::reflect


#include "particlesystem.h"
#include "argparser.h"

// ====================================================================
// Global Varibles
const double PI_CONST  = 3.1415926535;
// ====================================================================

ParticleSystem::ParticleSystem(ArgParser *a) : args(a){
  numParticles = 0;
  timestep = .000001;
  isBounded = true;
  initAmps  = 100.0;
  minAmps   = 0.1;
  velocity  = 340;
  particleRadius = 0.001;
  clusterRadius  = 0.01;
  clusterSize    = 4;
}

// ====================================================================
// ====================================================================

void ParticleSystem::setupVBOs() {
  HandleGLError("enter setupVBOs");
  setupPoints();
  HandleGLError("leaving setupVBOs");
}

// NOTE: The matrix that is passed in contains the transformations for
// the current camera/view position.  When drawing the objects in the
// scene you can apply additional transformations by multiplying your
// own matrices with this matrix before calling draw.

void ParticleSystem::drawVBOs(GLuint MatrixID,const glm::mat4 &m) {
  HandleGLError("enter drawVBOs");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &m[0][0]);
  drawPoints();
  HandleGLError("leaving drawVBOs");
}

// ====================================================================

double angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 norm){
    double angle = acos( glm::dot(a,b) / (glm::length(a) * glm::length(b)) );
    glm::vec3 c = glm::cross(norm,a);
    if(glm::dot(c,b) < 0)
        angle = -angle;

    return angle;
}

// ====================================================================
// ====================================================================
void ParticleSystem::update(){

  /*
   * Input : None
   * Output: This function will move all the particles by a timestep
   * Asumpt: There are particles to move
   * SideEf: Updates postition of particles
   */

    // For each ring of particles
    for(unsigned int i=0; i < particleRings.size(); i++){

      // Get pointer to that ring
      std::list<Particle*> * curRing = & particleRings[i];

      // Get iterator for this list
      std::list<Particle*>::iterator itr;

      // For each particle
      for (itr = curRing->begin(); itr != curRing->end(); ++itr){

        // Pointer to the particle
        Particle* curPart = *itr;

        // Are we below a threhold just kill and move to another
        if(curPart->getAmp() < minAmps){

          // I have the element after
          itr = curRing->erase(itr--);
          delete curPart;
          numParticles--;
          continue;
        }
        // Compute the new postion of the particle
        glm::vec3 pos = curPart->getPos();
        glm::vec3 dir = curPart->getDir();
        glm::vec3 axis = glm::vec3(1,0,0);
        glm::vec3 norm = glm::vec3(0,0,1);
        double radianAngle = angleBetween(axis,dir,norm);

        double vx = velocity * cos(radianAngle);
        double vy = velocity * sin(radianAngle);

        glm::vec3 newPos( pos.x + (vx * timestep) , pos.y + (vy * timestep), 0 );
        curPart->setPos(newPos);


        // Do we bounce, depends if we assume open space or closed space
        if(isBounded){
          // hit a vertical wall
          if(newPos.x < 0 || 1 < newPos.x ){
            glm::vec3 newDir(-1*dir.x, dir.y,0);
            curPart->setDir(newDir);
          }

          // Hit a horizontal wall
          if(newPos.y < 0 || 1 < newPos.y ){
            glm::vec3 newDir(dir.x, -1*dir.y, 0);
            curPart->setDir(newDir);
          }

        }//bounded

        // Debug ///////////////////////////////
        // curPart->print();
        // std::cout << "vx: " << vx << std::endl;
        // std::cout << "vy: " << vy << std::endl;
        // std::cout << "rad"  << radianAngle << std::endl;
        // End Debug ///////////////////////////////

        // Are we far away from other points enough to split?
        bool rightOutOfRange = false;
        bool leftOutOfRange  = false;

        // Check to the right if not at head
        if(itr != curRing->begin()){
          std::list<Particle*>::iterator itrRight = itr; itrRight--;
          Particle* rightPart = *itrRight;
          rightOutOfRange = 5 * particleRadius < glm::distance(curPart->getPos(), rightPart->getPos());
        }

        // Check to the left if not at the tail
        std::list<Particle*>::iterator itrLeft = itr; itrLeft++;
        if(itrLeft != curRing->end()){
          Particle* leftPart = *itrLeft;
          leftOutOfRange = 5 * particleRadius  < glm::distance(curPart->getPos(), leftPart->getPos());
        }

        // If either left or right is to far, split mofo!
        if(leftOutOfRange && rightOutOfRange){
          //std::cout << "Generating Children" << std::endl;

          // Get offset
          double angleBetweenParts = (2*PI_CONST)/clusterSize;
          double offsetAngle = angleBetweenParts/4.0;

          // Generate left particle
          double radianAngleRight = angleBetween(axis,dir,norm) - offsetAngle;
          double vxRight = velocity * cos(radianAngleRight);
          double vyRight = velocity * sin(radianAngleRight);
          glm::vec3 newPosRight( pos.x + (vxRight * timestep) , pos.y + (vyRight * timestep), 0 );
          glm::vec3 dirRight(newPosRight - pos);
          dirRight = glm::normalize(dirRight);
          Particle * newRightPart = new Particle(newPosRight,dirRight,curPart->getAmp()/3.0);


          // Generate right particle
          double radianAngleLeft = angleBetween(axis,dir,norm) + offsetAngle;
          double vxLeft = velocity * cos(radianAngleLeft);
          double vyLeft = velocity * sin(radianAngleLeft);
          glm::vec3 newPosLeft( pos.x + (vxLeft * timestep) , pos.y + (vyLeft * timestep),0 );
          glm::vec3 dirLeft(newPosLeft - pos);
          dirLeft = glm::normalize(dirLeft);
          Particle * newLeftPart = new Particle(newPosLeft,dirLeft,curPart->getAmp()/3.0);

          // Reduce amplage of middle particle
          curPart->setAmp(curPart->getAmp()/3.0);

          // Erease from list the old (middle entry) and add to Ring Structure
          // I have the element after
          itr = curRing->erase(itr);

          // Pushes the other values back
          curRing->insert(itr,newRightPart);
          curRing->insert(itr,curPart);
          curRing->insert(itr,newLeftPart);
          numParticles += 2;

          // Move back one so im at left newest
          itr--;

        }// Genereate child optinal

      }//ringloop for each particle

    }//vectorloop for each ring

    // You need to set up these points again
    setupVBOs();
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
    double radianAngle = ( 2 * PI_CONST ) / clusterSize;              // Debug 1.570796327

    // Create the dslist that will hold this list
    std::list<Particle*> ring;

    // For each particle I will make
    for(unsigned int i = 0; i < clusterSize; i++){

      // Get changes in postiton
      double dx = (cos(radianAngle * i) * clusterRadius); // 1 * .01 = .01
      double dy = (sin(radianAngle * i) * clusterRadius); // 0 * .01 = .0

      glm::vec2 pos_2d(x + dx, y + dy); // ( .05 + .01, .05 + .01  )

      // Set velolcity and normalize
      glm::vec2 dir_2d = glm::normalize(pos_2d - center);

      // Create this new particle
      glm::vec3 pos_3d(pos_2d.x, pos_2d.y, 0.0f);
      glm::vec3 dir_3d(dir_2d.x, dir_2d.y, 0.0f);

      Particle * newPart = new Particle(pos_3d,dir_3d,initAmps);

      // Debug
      // newPart->print();
      // std::cout << "dx dy: " << std::endl;
      // std::cout << dx << " " << dy << std::endl;

      // Add to collection
      ring.push_back(newPart);
      numParticles++;
    }

    // Push this newly made ring into my storage
    particleRings.push_back(ring);

}

// ====================================================================
// ====================================================================
void ParticleSystem::cleanupVBOs() {
  HandleGLError("enter cleanupVBOs");
  cleanupPoints();
  HandleGLError("leaving cleanupVBOs");
}

// ====================================================================
// ====================================================================

void ParticleSystem::setupPoints() {

  HandleGLError("enter setupPoints");

  // allocate space for the data

  VertexPosColor* points = new VertexPosColor[numParticles];

  for(unsigned int i = 0; i < particleRings.size(); i++){

     int index = 0;
     // Get the pointer to a ring stored in particleRings
     std::list<Particle*> * curRing = & particleRings[i];
     std::list<Particle*>::iterator itr;

     // For each particle get the position and save it
     for (itr = curRing->begin(); itr != curRing->end(); ++itr){
       Particle* curPart = *itr;
       glm::vec3 pos_2d = curPart->getPos();
       glm::vec4 pos_3d(pos_2d.x,pos_2d.y,pos_2d.z,1);
       points[index++] = VertexPosColor(pos_3d);
     }
   }


  // create a pointer for the VBO
  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);

  // Where we are storing these points
  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER,VboId); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * numParticles ,points,GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)sizeof(glm::vec4));
  
  // increase the size of the points slightly
  glPointSize(5.0);

  delete [] points;
  HandleGLError("leaving setupPoints");
}

void ParticleSystem::drawPoints() const {
  HandleGLError("enter drawPoints");
  // glDrawArrays draws the points
  glDrawArrays(
              GL_POINTS,    // The primitive you want to draw it as
              0,            // The start offset
              numParticles);  // The number of points

  HandleGLError("leaving drawPoints");
}

// ====================================================================
// ====================================================================

void ParticleSystem::cleanupPoints() {
  glDeleteBuffers(1, &VaoId);
  glDeleteBuffers(1, &VboId);
}

// ====================================================================
// ====================================================================
