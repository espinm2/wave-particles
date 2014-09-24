#include <cassert>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>// glm::normalize, glm::dot, glm::reflect


#include "particlesystem.h"
#include "argparser.h"

// Global Varibles
const long double PI_CONST  = atan(1)*4;

// ====================================================================
// Constructor
ParticleSystem::ParticleSystem(ArgParser *a) : args(a){
  numParticles = 0;
  timestep = .000005;
  isBounded = true;
  initAmps  = 10000.0;
  minAmps   = 0.1;
  velocity  = 340;
  // velocity  = 100;
  particleRadius = 0.01;
  clusterRadius  = 0.001;
  clusterSize    = 4;
}

// ====================================================================
// Update Simulation
void ParticleSystem::update(){

  /*
   * Input : None
   * Output: This function will move all the particles by a timestep
   * Asumpt: There are particles to move
   * SideEf: Updates postition of particles
   */

    // For each ring of particles
    for(unsigned int i=0; i < particleRings.size(); i++){

      std::list<Particle*> * curRing = & particleRings[i]; // Get pointer to that ring
      std::list<Particle*>::iterator itr;                  // Get iterator for this ring

      // Kill the ring if there is less then 3 paricles (FIXME)
      if(curRing->size() < clusterSize){
          for ( itr = curRing->begin(); itr != curRing->end(); ){
                delete * itr; // For each particle we delte it
                itr = curRing->erase(itr);
          }

          // Get rid of it from the main vector of list
          particleRings.erase(particleRings.begin()+i);
          i--;
          continue; // Move on to the next item

      }// kill ring


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
        long double radianAngle = angleBetween(axis,dir,norm);
        long double vx = velocity * cos(radianAngle);
        long double vy = velocity * sin(radianAngle);
        glm::vec3 newPos( pos.x + (vx * timestep) , pos.y + (vy * timestep), 0 );

        // Set the new position
        // Caution from now on on curPart->getPos() gives newPos
        curPart->setPos(newPos);


        // Do we bounce, depends if we assume open space or closed space
        long double distanceFromCenter = glm::distance(newPos,curPart->getCenter());
        if(isBounded){

          // hit a vertical wall
          if(newPos.x < 0 || 1 < newPos.x ){
            glm::vec3 newCenter = getPosCircle(distanceFromCenter,-1*radianAngle,newPos);
            glm::vec3 newDir(-1*dir.x, dir.y,0);
            curPart->setDir(newDir);
            curPart->setCenter(newCenter);
          }

          // Hit a horizontal wall
          if(newPos.y < 0 || 1 < newPos.y ){
            glm::vec3 newCenter = getPosCircle(distanceFromCenter,(-1*radianAngle)+PI_CONST,newPos);
            glm::vec3 newDir(dir.x, -1*dir.y, 0);
            curPart->setDir(newDir);
            curPart->setCenter(newCenter);
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

        // Check to the right if not at head also moves particles
        if(itr != curRing->begin()){
          std::list<Particle*>::iterator itrRight = itr; itrRight--;
          Particle* rightPart = *itrRight;
          rightOutOfRange = 5 * particleRadius < glm::distance(curPart->getPos(), rightPart->getPos());

        }else{
          // Check with the tail
          std::list<Particle*>::iterator itrRight = curRing->end(); itrRight--;
          Particle* rightPart = *itrRight;
          rightOutOfRange = 5 * particleRadius < glm::distance(curPart->getPos(), rightPart->getPos());
        }

        // If either left or right is to far, split mofo!
        if(leftOutOfRange || rightOutOfRange){

          // Get offset
          long double angleBetweenParts = (2*PI_CONST)/clusterSize;
          long double offsetAngle = angleBetweenParts/(3.0 * (curPart->getSplit() + 1)) ;
          // std::cout << curPart->getSplit()+1 << " " << distanceFromCenter << std::endl;

          // Generate left Particle
          long double radianAngleLeft = radianAngle + offsetAngle;
          glm::vec3 newPosLeft = getPosCircle(distanceFromCenter,radianAngleLeft,curPart->getCenter());
          glm::vec3 dirLeft(newPosLeft-curPart->getCenter());
          dirLeft = glm::normalize(dirLeft);
          Particle * newLeftPart = new Particle(newPosLeft,dirLeft,curPart->getAmp()/3.0, curPart->getSplit()+1);
          newLeftPart->setCenter(curPart->getCenter());

          // Generate Right Particle
          long double radianAngleRight = radianAngle - offsetAngle;
          glm::vec3 newPosRight = getPosCircle(distanceFromCenter,radianAngleRight,curPart->getCenter());
          glm::vec3 dirRight(newPosRight-curPart->getCenter());
          dirRight = glm::normalize(dirRight);
          Particle * newRightPart = new Particle(newPosRight,dirRight,curPart->getAmp()/3.0, curPart->getSplit()+1);
          newRightPart->setCenter(curPart->getCenter());

          // Reduce amplage of middle particle
          curPart->setAmp(curPart->getAmp()/3.0);
          curPart->setSplit(curPart->getSplit()+1);

          /*
          // Debug ================================================
          std::cout << "Debug ============================================= " << std::endl;
          // std::cout << "Left Angle " << radianAngleLeft << "-> " << newPosLeft.x <<", "<< newPosLeft.y << std::endl;
          // std::cout << "Same Angle " << radianAngle << "-> " << newPos.x <<", "<< newPos.y << std::endl;
          // std::cout << "Righ Angle " << radianAngleRight << "-> " << newPosRight.x <<", "<< newPosRight.y << std::endl;
          newLeftPart->print();
          curPart->print();
          newRightPart->print();
          std::cout << std::endl;
          // End Debug ============================================
          */


          // Erease from list the old (middle entry) and add to Ring Structure
          // I have the element after
          itr = curRing->erase(itr);

          // Pushes the other values back
          itr = curRing->insert(itr,newLeftPart);
          itr = curRing->insert(itr,curPart);
          itr = curRing->insert(itr,newRightPart);
          numParticles += 2;

          // Move back one so im at left newest
          itr++;
          itr++;

        }// Genereate child optinal

      }//ringloop for each particle

    }//vectorloop for each ring

    // You need to set up these points again
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
       double split = (2*curPart->getSplit()*curPart->getSplit())/255.0;
       glm::vec4 color(1,1-split,.2, 1);
       points[index++] = VertexPosColor(pos_3d, color);
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
  glPointSize(3.0);

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
              numParticles);  // The number of points

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
double ParticleSystem::angleBetween(glm::vec3 a, glm::vec3 b, glm::vec3 norm){
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

      Particle * newPart = new Particle(pos_3d,dir_3d,initAmps,0);

      newPart->setCenter(glm::vec3(center.x,center.y,0));

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

glm::vec3 ParticleSystem::getPosCircle(double radius, double radAngle, glm::vec3 center){
  /*
   * Input : center of a circle, the angle relative to X axis, and radius
   * Output: Point on a circle with those attrabutes
   * Asumpt: radAngle is radians and like a unit circle, 2D
   * SideEf: None
   */
    return glm::vec3(center.x + radius * cos(radAngle), center.y + radius * sin(radAngle),0);
}
