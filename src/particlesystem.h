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
#include "wall.h"
#include "vectors.h"
#include "grid.h"


/*
 *╔╦╗╔═╗╔╦╗╔═╗
 * ║ ║ ║ ║║║ ║
 * ╩ ╚═╝═╩╝╚═╝
 *
 * [x] Implement Grid::placeWallInGrid(..)
 * [ ] Move these VBO classes into another file
 * [ ] Clean up random static var CELLS_NUM_VBO
 * [ ] Fix Issues with Cubes not changing color + rendering
 * [?] What is glUniform1i(GLCanvas::ColormodeId, 1)
 * [ ] Implement ParticleSystem::calculateBounces(..)
 * [ ] Please fix code so we can access particles by refrence when possible
 *
 */

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

class VBOPosNormalColor{

public:
  VBOPosNormalColor() {}

  VBOPosNormalColor(const glm::vec3 &p) {
    x = p.x; y = p.y; z = p.z;
    nx = 1; ny = 0; nz = 0;
    r = 0; g = 0; b = 0;
  }
  VBOPosNormalColor(const glm::vec3 &p, const glm::vec3 &n, const glm::vec3 &c) {
    x = p.x; y = p.y; z = p.z;
    nx = n.x; ny = n.y; nz = n.z;
    r = c.x; g = c.y; b = c.z;
  }
  float x, y, z;    // position
  float nx, ny, nz; // normal
  float r, g, b;    // color

};

class VBOIndexedTri{
public:
  VBOIndexedTri() {}
  VBOIndexedTri(unsigned int a, unsigned int b, unsigned int c) {
    verts[0] = a;
    verts[1] = b;
    verts[2] = c;
  }
  unsigned int verts[3];
};
// holds simulation constants
class ArgParser;

class ParticleSystem {


public:

  // CONSTRUCTOR
  ParticleSystem(ArgParser *a);

  // DESTRUCTOR
  ~ParticleSystem();

  // SIMULATION FUCTIONS

  // Resetting simulation // TODO
  void reset(){ std::cout << "Not Implemented Yet" << std::endl; assert(false);}

  void createWall();

  // Creates wave at world coordinates (x,y)
  void createWave(double x, double y);

  // returns (x,y,z) position of a particle given circle attr
  Vec3f getPosCircle(double radius, double radAngle, const Vec3f center);

  // returns how many particles we have
  unsigned int getSize(){ return particleVec.size(); }

  // Returns true if ww need to split
  bool outOfRangeGrid(Particle * p);

  // What the fuck is this ?!
  std::vector<Particle * >::iterator removeParticle(std::vector<Particle *>::iterator);

  // Moves all particles by a time unit, uses pos attr
  void moveParticle(Particle * p);

  // Change particle p to reflect a bounce if we hit wall, else don't change p
  void calculateBounces(Particle * &p);

  // Creates 3 new particles from split at the same radius
  void splitParticle(Particle * p, Particle * a, Particle * b, Particle * c);

  // Returns the thredhold at which we split
  double splitDistance(){return args->particleRadius * .5;}

  // Setup walls for both VBO and Grid
  void loadWallsFromFile(std::string input_file);


  // HELPER FUNCTIONS FOR RENDERING
  void setupVBOs();
  void drawVBOs(GLuint MatrixID,const glm::mat4 &MVP);
  void update();
  void cleanupVBOs();

private:

  // private helper functions for VBO setup
  void setupPoints();
  void drawPoints() const;

  void setupBBox();
  void drawBBox() const;

  void setupWalls();
  void drawWalls() const;

  void setupCellVis();
  void drawCellsVis();


  void setupCubeVBO(const glm::vec4 pts[8],
      const glm::vec4 &color,
      std::vector<VertexPosColor> &faces);


  // REPRESENTATION
  std::vector<Particle*> particleVec;
  std::vector<Wall*> wallVec;
  Grid particleGrid;
  ArgParser *args;

  // Number of VAOs and VBOs (one per object)
  static const int NumVAO = 4;
  static const int NumVBO = NumVAO;

  GLuint VaoId[NumVAO];
  GLuint VboId[NumVBO];

};


#endif
