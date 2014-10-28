#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>

#include "vectors.h"
#include "MersenneTwister.h"

// ====================================================================
// ====================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {

    // Call default values
    DefaultValues();


    for (int i = 1; i < argc; i++) {
      if (argv[i] == std::string("-unbound")) {

          i++; assert (i < argc);
          isBounded = false;

      } else if (argv[i] == std::string("-verbose")) {

          i++; assert (i < argc);
          verbose = true;

      } else if (argv[i] == std::string("-timestep")) {

          i++; assert (i < argc);
          timestep = atoi(argv[i++]);

      } else if (argv[i] == std::string("-size")) {

          i++; assert (i < argc);
          width = atoi(argv[i++]);
          height = atoi(argv[i]);

      } else if (argv[i] == std::string("-initAmps")) {

          i++; assert (i < argc);
          initAmps = atoi(argv[i++]);

      } else if (argv[i] == std::string("-minAmps"))  {

          i++; assert (i < argc);
          minAmps = atoi(argv[i++]);

      } else if (argv[i] == std::string("-velocity"))  {

          i++; assert (i < argc);
          velocity = atoi(argv[i++]);

      } else if (argv[i] == std::string("-worldRange")) {

          i++; assert (i < argc);
          worldRange = atoi(argv[i++]);

      } else if (argv[i] == std::string("-gridDivisions")) {

          i++; assert (i < argc);
          gridDivisions = atoi(argv[i++]);

      } else {
	std::cout << "ERROR: unknown command line argument " 
		  << i << ": '" << argv[i] << "'" << std::endl;
	exit(1);
      }
    }
  }


  void DefaultValues() {

    // Simulation options
    animate    = true;
    isBounded  = true;
    verbose = false;

    // Window options + shader loc
    width  = 512;
    height = 512;
    path   = ".";

    // Simulations values
    timestep        = .2;
    initAmps        = 1000;
    minAmps         = 10;
    velocity        = 1;
    particleRadius  = 2;
    clusterRadius   = .1;
    initClusterSize = 4;
    worldRange      = 100;
    gridDivisions   = 20;

    //Triggers + Temp Var
    resetRequest    = false;
    createWallRequest = false;

  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)


  // Simulation options
  bool animate;
  bool isBounded;
  bool verbose;

  // Window options + shader loc
  unsigned int width;
  unsigned int height;
  std::string path;

  // Simulations values
  double timestep;
  double initAmps;
  double minAmps;
  double velocity;
  double particleRadius;
  double clusterRadius;
  double initClusterSize;
  unsigned int worldRange;
  unsigned int gridDivisions;

  // Triggers + Temp Var
  bool resetRequest;
  bool createWallRequest;
  Vec3f tempWallPosA;
  Vec3f tempWallPosB;



};

#endif
