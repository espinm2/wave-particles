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
   * Output: This function will update our particle simuations
   * Asumpt: There are particles to move
   * SideEf: Updates postition of particles/ removes particles
   */

  std::vector<Particle *> newParticles;

  std::vector<Particle *>::iterator iter;
  for(iter = particleRings.begin(); iter != particleRings;){

     Particle * curPart = (*iter);

    // Are we below a threhold just kill and move to another
    if(curPart->getAmp() < minAmps){

      // Kill this partcile and move to next
      iter = removeParticle(iter);
      continue;
    }

    // Particles are beyond a threshold init a split
    if(outOfRange(curPart)){

        // Create 3 new particles and assign them
        Particle * a = new Particle;
        Particle * b = new Particle;
        Particle * c = new Particle;

        curPart->splitParticle(a,b,c);

        // Move them a timestep
        moveParticle(a);
        moveParticle(b);
        moveParticle(c);

        // Push them into newParticles list for l8 addition
        newParticles.push_back(a);
        newParticles.push_back(b);
        newParticles.push_back(c);


        // Remove and kill the current particle
        iter = removeParticle(iter);

    }else{

        // Update postiton and move to next particle
        moveParticle(curPart);
        iter++;
    }

  } //forloop


  // Add into the main vector those new particles
  for( int i = 0; i < newParticles.size(); i++)
      particleRings.push_back(newParticles[i]);

  // Recreate the VBOs
  setupVBOs();

}//update

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

     Particle* curPart = particleRadius[i];

     // Getting posititons
     glm::vec3 pos_2d = curPart->getPos();
     glm::vec4 pos_3d(pos_2d,1);

     // Color visualzations
     double split = (2*curPart->getSplit()*curPart->getSplit())/255.0;
     glm::vec4 color(1,1-split,.2, 1);

     // Adding to VBO
     points[index++] = VertexPosColor(pos_3d, color);

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
      particleRings.push_back(newPart);
      numParticles++;
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
