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
  isBounded = false;
  initAmps  = 10.0;
  minAmps   = 0.001;
  velocity  = 340;
  particleRadius = 1.0;
  clusterRadius  = 10.0;
  clusterSize    = 6;
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
// ====================================================================

void ParticleSystem::update(){
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
    double radianAngle = ( 2 * PI_CONST ) / clusterSize;

    // Create the dslist that will hold this list
    std::list<Particle*> ring;

    // For each particle I will make
    for(unsigned int i = 0; i < clusterSize; i++){

      // Get changes in postiton
      int dx = (int)(cos(radianAngle * i) * clusterRadius);
      int dy = (int)(sin(radianAngle * i) * clusterRadius);
      glm::vec2 pos_2d(x + dx, y + dy);

      // Set velolcity and normalize
      glm::vec2 dir_2d = glm::normalize(pos_2d - center);

      // Create this new particle
      glm::vec3 pos(pos_2d.x, pos_2d.y, 0.0f);
      glm::vec3 dir(dir_2d.x, dir_2d.y, 0.0f);

      Particle * newPart = new Particle(pos,dir,initAmps);

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
  /*
  // VertexPosColor* points = new VertexPosColor[numParticles + 8];

  for(unsigned int i = 0; i < particleRings.size(); i++){

     // Get the pointer to a ring stored in particleRings
     std::list<Particle*> * curRing = & particleRings[i];
     std::list<Particle*>::iterator itr;

     // For each particle get the position and save it
     for (itr = curRing->begin(); itr != curRing->end(); ++itr){
       Particle* curPart = *itr;

       glm::vec3 pos_2d = curPart->getPos();
       glm::vec4 pos_3d(pos_2d.x,pos_2d.y,pos_2d.z,1);
       points[i] = VertexPosColor(pos_3d);
     }
   }
   */

  /*
  glm::vec4 one(0,0,0,1);
  glm::vec4 two(0.5,0,0,1);
  glm::vec4 three(0,0.5,0,1);
  glm::vec4 four(0.5,0.5,0,1);

  glm::vec4 five(0,0,1,1);
  glm::vec4 six(1,0,1,1);
  glm::vec4 seven(1,1,1,1);
  glm::vec4 eight(0,1,1,1);

  points[0] = VertexPosColor(one);
  points[1] = VertexPosColor(two);
  points[2] = VertexPosColor(three);
  points[3] = VertexPosColor(four);
  points[4] = VertexPosColor(five);
  points[5] = VertexPosColor(six);
  points[6] = VertexPosColor(seven);
  points[7] = VertexPosColor(eight);
  */

  unsigned int  len = 4;
  VertexPosColor* points = new VertexPosColor[len];
  glm::vec4 pos1(0.0f,0.0f,0.0f,1.0f);
 // glm::vec4 pos2(0.0f,0.0f,0.0f,1.0f);
  glm::vec4 pos3(0.0f,0.0f,0.0f,1.0f);
  glm::vec4 pos2(1.0f,0.0f,0.0f,1.0f);
  // glm::vec4 pos3(0.0f,1.0f,0.0f,1.0f);
  glm::vec4 pos4(1.0f,1.0f,0.0f,1.0f);

  points[0] = VertexPosColor(pos1);
  points[1] = VertexPosColor(pos2);
  points[2] = VertexPosColor(pos3);
  points[3] = VertexPosColor(pos4);

  // create a pointer for the VBO
  glGenVertexArrays(1, &VaoId);
  glBindVertexArray(VaoId);

  // Where we are storing these points
  glGenBuffers(1, &VboId);
  glBindBuffer(GL_ARRAY_BUFFER,VboId); 
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * len ,points,GL_STATIC_DRAW);

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
              4);  // The number of points

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
