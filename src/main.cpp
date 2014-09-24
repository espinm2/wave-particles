// Graphics Library Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "argparser.h"
#include "particle.h"
#include "glCanvas.h"
#include "camera.h"
#include "particlesystem.h"

// ====================================================================
// ====================================================================

int main(int argc, char *argv[]) {

  // parse the command line arguments
  ArgParser args(argc, argv);

  // create the particlesystem object
  ParticleSystem partsys(&args);
  GLCanvas::initialize(&args,&partsys);
  
  glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( args.path+"/shader.vertexshader", args.path+"/shader.fragmentshader" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");


  while (!glfwWindowShouldClose(GLCanvas::window))  {

    // assert(partsys.getSize() > 0); // DEBUG: Are we creating stuff in wave particles
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(programID);

    GLCanvas::camera->glPlaceCamera();


    // Build the matrix to position the camera based on keyboard and mouse input
    glm::mat4 ProjectionMatrix = GLCanvas::camera->getProjectionMatrix();
    glm::mat4 ViewMatrix = GLCanvas::camera->getViewMatrix();
    glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    
    // pass the matrix to the draw routines (for further editing)
    partsys.drawVBOs(MatrixID,MVP);

    // Animate do it 10 times before next render
    if(args.animate){
        partsys.update(); // std::cout << partsys.getSize() << std::endl;
    }


    // Swap buffers
    glfwSwapBuffers(GLCanvas::window);
    glfwPollEvents();  

  }
  
  partsys.cleanupVBOs();
  glDeleteProgram(programID);
  
  // Close OpenGL window and terminate GLFW
  glfwDestroyWindow(GLCanvas::window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

// ====================================================================
// ====================================================================

