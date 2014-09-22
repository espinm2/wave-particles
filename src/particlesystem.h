#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include <cassert>
#include <vector>
#include <string>
#include <list>

#include "glCanvas.h"
#include "particle.h"

class ArgParser;

// ====================================================================
// ====================================================================
// class to manage the data and drawing for an "iterated function system"

class ParticleSystem {

  // ASSIGNMENT: add whatever additional variables & functions you need

public:

  // CONSTRUCTOR
  ParticleSystem(ArgParser *a);

  // HELPER FUNCTIONS FOR RENDERING
  void setupVBOs();
  void drawVBOs(GLuint MatrixID,const glm::mat4 &MVP);
  void update();
  void cleanupVBOs();

  // Simulation Functions
  void createWave(double x, double y);
  unsigned int getSize(){ return numParticles; }

private:

  // private helper functions for VBOs
  void setupPoints();
  void drawPoints() const;
  void cleanupPoints();

  // REPRESENTATION
  ArgParser *args;
  GLuint VaoId;
  GLuint VboId;

  std::vector<std::list<Particle*> > particleRings; // Where we keep particles
  unsigned int numParticles;                        // Update to keep accurate

  // Tune to change simulation behavior
  bool isBounded;
  double initAmps;
  double minAmps;
  double velocity;
  double particleRadius;
  double clusterRadius;
  unsigned int clusterSize;

};

// ====================================================================
// some helper stuff for VBOs
class VertexPosColor {
public:
  // by default vertices are set to black
  VertexPosColor(const glm::vec4 &pos=glm::vec4(0,0,0,1), 
                 const glm::vec4 &color=glm::vec4(0,0,0,1)) :
    XYZW(pos),RGBA(color) {}
  glm::vec4 XYZW;
  glm::vec4 RGBA;
};
// ====================================================================

#endif
