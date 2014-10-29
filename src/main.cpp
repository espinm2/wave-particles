// Graphics Library Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "vectors.h"
#include "argparser.h"
#include "particle.h"
#include "glCanvas.h"
#include "camera.h"
#include "MersenneTwister.h"
#include "particlesystem.h"

/*
 *╔╦╗╔═╗╔╦╗╔═╗
 * ║ ║ ║ ║║║ ║
 * ╩ ╚═╝═╩╝╚═╝
 *
 * [x] Implement Grid::placeWallInGrid(..)
 * [x] Move these VBO classes into another file
 * [x] Clean up random static var CELLS_NUM_VBO
 * [x] Fix Issues with Cubes not changing color + rendering
 * [?] What is glUniform1i(GLCanvas::ColormodeId, 1)
 * [ ] Implement ParticleSystem::calculateBounces(..)
 * [ ] Please fix code so we can access particles by refrence when possible
 *
 */

int main(int argc, char *argv[]) {

  // parse the command line arguments
  ArgParser args(argc, argv);

  // create the particlesystem object
  ParticleSystem partsys(&args);
  GLCanvas::initialize(&args,&partsys);

  // Blending + Paramaters
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Background color
  glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

  // Not need because 2D Simulations
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);


  // Setting Points Size
  glPointSize(3.0);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( args.path+"/shader.vertexshader", args.path+"/shader.fragmentshader" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  while (!glfwWindowShouldClose(GLCanvas::window)){
    
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // No GL_DEPTH_BUFFER_BIT

      glUseProgram(programID); // Use these shaders

      // Camera and View Matrixes
      GLCanvas::camera->glPlaceCamera();

      // Build the matrix to position the camera based on keyboard and mouse input
      glm::mat4 ProjectionMatrix = GLCanvas::camera->getProjectionMatrix();
      glm::mat4 ViewMatrix = GLCanvas::camera->getViewMatrix();
      glm::mat4 ModelMatrix = glm::mat4(1.0);
      glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

      // Pass the matrix to the draw routines (for further editing)
      partsys.drawVBOs(MatrixID,MVP);

      // Request Handling
      if(args.resetRequest){
          partsys.reset();
      }

      if(args.createWallRequest){
          partsys.createWall();
      }

      // Updating Simulation
      if(args.animate){
        partsys.update();
      }

      // Swap buffers
      glfwSwapBuffers(GLCanvas::window);
      glfwPollEvents();

  }// mainloop

  
  // Cleaning up
  partsys.cleanupVBOs();
  glDeleteProgram(programID);
  
  // Close OpenGL window and terminate GLFW
  glfwDestroyWindow(GLCanvas::window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

// ====================================================================

