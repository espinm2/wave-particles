#ifndef __ARG_PARSER_H__
#define __ARG_PARSER_H__

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <string>
#include "MersenneTwister.h"

// ====================================================================
// ====================================================================

class ArgParser {

public:

  ArgParser() { DefaultValues(); }

  ArgParser(int argc, char *argv[]) {
    DefaultValues();
    // parse the command line arguments
    for (int i = 1; i < argc; i++) {
      if (argv[i] == std::string("-iters")) {
	i++; assert (i < argc); 
	iters = atoi(argv[i]);
      } else if (argv[i] == std::string("-size")) {
	i++; assert (i < argc); 
    width = atoi(argv[i++]);
    height = atoi(argv[i]);
      } else {
	std::cout << "ERROR: unknown command line argument " 
		  << i << ": '" << argv[i] << "'" << std::endl;
	exit(1);
      }
    }
  }

  void DefaultValues() {
    animate = false;
    iters = 3;
    width = 400;
    height = 400;
    path = "."; // so you can find the shaders
  }

  // ==============
  // REPRESENTATION
  // all public! (no accessors)

  bool animate;
  std::string path;
  int iters;
  int width;
  int height;

  // default initialization
  MTRand mtrand;
};

#endif
