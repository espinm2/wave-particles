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
// Bugs
// (+) Inaccurate creation of splits, offsetAngle could be better
// (+) Particles will not render
// (+) Particles will no die
// ====================================================================

int main(int argc, char *argv[]) {

  // parse the command line arguments
  ArgParser args(argc, argv);

  // create the particlesystem object
  ParticleSystem partsys(&args);
  GLCanvas::initialize(&args,&partsys);
  
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( args.path+"/shader.vertexshader", args.path+"/shader.fragmentshader" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");

  // Debug ///////////////////////////////////////////////

  partsys.createWave(25,25);

  /*
  glm::vec3 pos(.5,.5,0);
  glm::vec3 dir(1,0,0);
  glm::vec3 cen(.2,.5,0);


  Particle * debug = partsys.particleRings[0];
  Particle * a = new Particle;
  Particle * b = new Particle;
  Particle * c = new Particle;

  partsys.splitParticle(debug,a,b,c);

  std::cout << "Particle A " << *a << std::endl;
  std::cout << "Particle B " << *b << std::endl;
  std::cout << "Particle C " << *c << std::endl;


  partsys.particleRings.push_back(a);
  partsys.particleRings.push_back(b);
  partsys.particleRings.push_back(c);

  // Works

  partsys.removeParticle(partsys.particleRings.begin()); // this could be it

  // Moving so i can split later
  partsys.moveParticle(a);
  partsys.moveParticle(a);
  partsys.moveParticle(a);
  partsys.moveParticle(a);
  partsys.moveParticle(a);

  Particle * aa = new Particle;
  Particle * ab = new Particle;
  Particle * ac = new Particle;

  partsys.splitParticle(a, aa, ab, ac);

  std::cout << "Particle A " << *a << std::endl;
  std::cout << "Particle AA " << *aa << std::endl;
  std::cout << "Particle AB " << *ab << std::endl;
  std::cout << "Particle AC " << *ac << std::endl;

  partsys.particleRings.push_back(aa);
  partsys.particleRings.push_back(ab);
  partsys.particleRings.push_back(ac);

  // partsys.removeParticle(partsys.particleRings.begin()); // this could be it

  partsys.setupVBOs();

  */

  // End Debug ///////////////////////////////////////////////


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

    if(args.animate){
        partsys.update();
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

