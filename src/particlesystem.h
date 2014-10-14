#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

#include <cassert>
#include <vector>
#include <string>
#include <list>

#include "glCanvas.h"
#include "particle.h"
#include "vectors.h"
#include "grid.h"

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
  Vec3f getPosCircle(double radius, double radAngle, const Vec3f center);
  unsigned int getSize(){ return particleRings.size(); }
  double angleBetween(Vec3f a, Vec3f b, Vec3f norm);
  bool outOfRange(Particle * p);
  std::vector<Particle * >::iterator removeParticle(std::vector<Particle *>::iterator);
  void moveParticle(Particle * p);
  void splitParticle(Particle * p, Particle * a, Particle * b, Particle * c);



private:

  // private helper functions for VBOs
  void setupPoints();
  void drawPoints() const;
  void cleanupPoints();

  // REPRESENTATION
  std::vector<Particle*> particleRings;
  Grid particleGrid;
  ArgParser *args;
  GLuint VaoId;
  GLuint VboId;


  // Tune to change simulation behavior
  double timestep;
  bool isBounded;
  double initAmps;
  double minAmps;
  double velocity;
  double particleRadius;
  double clusterRadius;
  unsigned int initClusterSize;

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

#endif
