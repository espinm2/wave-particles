#ifndef _PARTICLESYSTEM_H_
#define _PARTICLESYSTEM_H_

// GLM includes
#include <glm/gtc/type_ptr.hpp>
#include <glm/geometric.hpp>

// STL includes
#include <cassert>
#include <vector>
#include <string>
#include <list>
#include <fstream>
#include <math.h>

// Class includes
#include "MersenneTwister.h"
#include "argparser.h"
#include "glCanvas.h"
#include "particle.h"
#include "vectors.h"
#include "grid.h"


// holds simulation constants
class ArgParser;

class ParticleSystem {


public:

  // CONSTRUCTOR
  ParticleSystem(ArgParser *a);

  // DESTRUCTOR
  ~ParticleSystem();

  // HELPER FUNCTIONS FOR RENDERING
  void setupVBOs();
  void drawVBOs(GLuint MatrixID,const glm::mat4 &MVP);
  void update();
  void cleanupVBOs();

  // SIMULATION FUCTIONS

  // Creates wave at world coordinates (x,y)
  void createWave(double x, double y);

  // returns (x,y,z) position of a particle given circle attr
  Vec3f getPosCircle(double radius, double radAngle, const Vec3f center);

  // returns how many particles we have
  unsigned int getSize(){ return particleVec.size(); }

  // Get the angle between vectors
  double angleBetween(Vec3f a, Vec3f b, Vec3f norm);

  // Returns true if ww need to split
  bool outOfRangeGrid(Particle * p);

  // What the fuck is this ?!
  std::vector<Particle * >::iterator removeParticle(std::vector<Particle *>::iterator);

  // Moves all particles by a time unit, uses pos attr
  void moveParticle(Particle * p);

  // Creates 3 new particles from split at the same radius
  void splitParticle(Particle * p, Particle * a, Particle * b, Particle * c);


private:

  // private helper functions for VBOs
  void setupPoints();
  void drawPoints() const;
  void cleanupPoints();

  // REPRESENTATION
  std::vector<Particle*> particleVec;
  Grid particleGrid;
  ArgParser *args;
  GLuint VaoId;
  GLuint VboId;

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
