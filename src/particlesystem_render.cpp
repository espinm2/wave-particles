#include "particlesystem.h"

// ╦  ╦╔╗ ╔═╗  ╦╔╦╗╔═╗
// ╚╗╔╝╠╩╗║ ║  ║ ║║╚═╗
//  ╚╝ ╚═╝╚═╝  ╩═╩╝╚═╝
#define GL_PARTICLES   0
#define GL_BOUNDINGBOX 1
#define GL_WALL        2
#define GL_CELLS       3

// TODO remove for 3D BBox
#define BBOX_VERTICES_NUM 5

// ╔═╗╔═╗╔╗╔╔═╗╦═╗╔═╗╦    ╦  ╦╔╗ ╔═╗
// ║ ╦║╣ ║║║║╣ ╠╦╝╠═╣║    ╚╗╔╝╠╩╗║ ║
// ╚═╝╚═╝╝╚╝╚═╝╩╚═╩ ╩╩═╝   ╚╝ ╚═╝╚═╝

void ParticleSystem::setupVBOs() {

  HandleGLError("enter setupVBOs");

  // Setting up Ids for VAO
  glGenVertexArrays(NumVAO,VaoId);

  // Getting name for the VBOs we will use
  glGenBuffers(NumVBO, VboId);


  // Setting up objects to render
  setupPoints();
  setupBBox();
  setupWalls();
  if(args->cell_vis) setupCellVis();


  HandleGLError("leaving setupVBOs");
}

void ParticleSystem::drawVBOs(GLuint MatrixID,const glm::mat4 &m) {

  HandleGLError("enter drawVBOs");
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &m[0][0]);
  drawPoints();
  drawBBox();
  drawWalls();

  if(args->cell_vis)drawCellsVis();
  HandleGLError("leaving drawVBOs");
}

void ParticleSystem::cleanupVBOs() {

  HandleGLError("enter cleanupVBOs");
  glDeleteBuffers(NumVAO, VaoId);
  glDeleteBuffers(NumVBO, VboId);
  HandleGLError("leaving cleanupVBOs");
}

// ╔═╗╔═╗╦═╗╔╦╗╦╔═╗╦  ╔═╗╔═╗
// ╠═╝╠═╣╠╦╝ ║ ║║  ║  ║╣ ╚═╗
// ╩  ╩ ╩╩╚═ ╩ ╩╚═╝╩═╝╚═╝╚═╝

void ParticleSystem::setupPoints() {

  HandleGLError("enter setupPoints");

  // allocate space for the data

  VertexPosColor* points = new VertexPosColor[particleVec.size()];

  unsigned int countSplit2 = 0;
  for(unsigned int i = 0; i < particleVec.size(); i++){

     Particle * curPart = particleVec[i];


     // Getting posititons
     Vec3f pos_2d = curPart->getPos();
     //glm::vec4 pos_3d(pos_2d.x(), pos_2d.y(), randGen.randInt(1), 1); //locked for 2d case
     glm::vec4 pos_3d(pos_2d.x(), pos_2d.y(), args->worldRange/2.0, 1); //locked for 2d case

     glm::vec4 color;
     // even
     int split = curPart->getSplit();

     if(split%3 == 0){
         // color = glm::vec4((i/(double)particleRings.size()),0,0,1);//red
         color = glm::vec4(1,0,0,0.5);//red
     }else if(split%3 == 1){
         // color = glm::vec4(0,0,(i/(double)particleRings.size()),1);//blue
         color = glm::vec4(0,0,1,0.5);//blue
     } else if(split%3 == 2){
         // color = glm::vec4(0,(i/(double)particleRings.size()),0,1);//green
         color = glm::vec4(0,1,0,0.5);//green
         countSplit2++;
     }else{
         color = glm::vec4(1,0,1,0.5);//black
     }


     // Adding to VBO
     points[i] = VertexPosColor(pos_3d,color);

   }


  // Working  with Particles VAO
  glBindVertexArray(VaoId[GL_PARTICLES]);

  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_PARTICLES]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * particleVec.size(), points,GL_STREAM_DRAW);

  // For postion
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);

  // For color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)sizeof(glm::vec4));



  delete [] points;
  HandleGLError("leaving setupPoints");
}

void ParticleSystem::drawPoints() const {

  glBindVertexArray(VaoId[GL_PARTICLES]);
  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_PARTICLES]);

  HandleGLError("enter drawPoints");
  glDrawArrays(
              GL_POINTS,    // The primitive you want to draw it as
              0,            // The start offset
              particleVec.size());  // The number of points

  HandleGLError("leaving drawPoints");
}

// ╔╗ ╔═╗╦ ╦╔╗╔╔╦╗╦╔╗╔╔═╗  ╔╗ ╔═╗═╗ ╦
// ╠╩╗║ ║║ ║║║║ ║║║║║║║ ╦  ╠╩╗║ ║╔╩╦╝
// ╚═╝╚═╝╚═╝╝╚╝═╩╝╩╝╚╝╚═╝  ╚═╝╚═╝╩ ╚═


void ParticleSystem::setupBBox(){
  HandleGLError("enter setupBBox");

  // allocate space for the data

  // Manual Bounding Box
  VertexPosColor* vertices = new VertexPosColor[BBOX_VERTICES_NUM];

  int s = args->worldRange;
  int z = s / 2;


  // Adding to VBO
  vertices[0]  = VertexPosColor(glm::vec4(0,0,z,1),glm::vec4(1,1,1,1));
  vertices[1]  = VertexPosColor(glm::vec4(0,s,z,1),glm::vec4(1,1,1,1));
  vertices[2]  = VertexPosColor(glm::vec4(s,s,z,1),glm::vec4(1,1,1,1));
  vertices[3]  = VertexPosColor(glm::vec4(s,0,z,1),glm::vec4(1,1,1,1));
  vertices[4]  = VertexPosColor(glm::vec4(0,0,z,1),glm::vec4(1,1,1,1));



  // Working  with Particles VAO
  glBindVertexArray(VaoId[GL_BOUNDINGBOX]);

  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_BOUNDINGBOX]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * BBOX_VERTICES_NUM, vertices,GL_STATIC_DRAW);

  // For postion
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);

  // For color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)sizeof(glm::vec4));


  delete [] vertices;
  HandleGLError("leaving setupBBox");


}


void ParticleSystem::drawBBox() const {

  HandleGLError("enter drawBBOX");
  glBindVertexArray(VaoId[GL_BOUNDINGBOX]);
  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_BOUNDINGBOX]);

  HandleGLError("enter drawBBOX");
  glDrawArrays(
              GL_LINE_STRIP,    // The primitive you want to draw it as
              0,            // The start offset
              BBOX_VERTICES_NUM);  // The number of points

  HandleGLError("leaving drawBBOX");
}


// ╔═╗╔═╗╦  ╦    ╦  ╦╦╔═╗╦ ╦╔═╗╦  ╔═╗
// ║  ║╣ ║  ║    ╚╗╔╝║╚═╗║ ║╠═╣║  ╚═╗
// ╚═╝╚═╝╩═╝╩═╝   ╚╝ ╩╚═╝╚═╝╩ ╩╩═╝╚═╝

void ParticleSystem::setupCellVis() {

    HandleGLError("enter setupCellVis");
    // Load everything into cell_vis_vbo for glCommands
    std::vector<VertexPosColor> cell_vis_vbo;
    int range = args->gridDivisions;
    double dx,dy,dz;
    dx = dy = dz = args->worldRange / (double)args->gridDivisions;
    int k = range/2.0-1;

    // setupCubeVBO(pts2,white,cell_vis_vbo);
    for(int i = 0; i < range; i++){
      for(int j = 0; j < range; j++){

          //if(particleGrid.getCell(i,j)->getWalls().size() > 0){


          int particles_in_cell = particleGrid.getCell(i,j)->getParticles().size();
          glm::vec4 color;

          glm::vec4 pts[8] = { glm::vec4((i+0.1)*dx,(j+0.1)*dy,(k+0.1)*dz,1.0),
                               glm::vec4((i+0.1)*dx,(j+0.1)*dy,(k+0.9)*dz,1.0),
                               glm::vec4((i+0.1)*dx,(j+0.9)*dy,(k+0.1)*dz,1.0),
                               glm::vec4((i+0.1)*dx,(j+0.9)*dy,(k+0.9)*dz,1.0),
                               glm::vec4((i+0.9)*dx,(j+0.1)*dy,(k+0.1)*dz,1.0),
                               glm::vec4((i+0.9)*dx,(j+0.1)*dy,(k+0.9)*dz,1.0),
                               glm::vec4((i+0.9)*dx,(j+0.9)*dy,(k+0.1)*dz,1.0),
                               glm::vec4((i+0.9)*dx,(j+0.9)*dy,(k+0.9)*dz,1.0) };

          if(particleGrid.getCell(i,j)->getWalls().size() > 0){
              if(particles_in_cell > 0){
                color = glm::vec4(0.9,0.2,0.1,.1);
              }else{
                color = glm::vec4(0.2,0.9,0.5,.1);
              }
          }else if(particles_in_cell > 0){
              color = glm::vec4(0.8,0.9,0.5*(particles_in_cell/20.0),.1);
          }else{
              color = glm::vec4(0.2,0.3,0.5,.1);
          }

          setupCubeVBO(pts,color,cell_vis_vbo);
         // }
      }
    }




  // Bind the VAO
  glBindVertexArray(VaoId[GL_CELLS]);

  // Bind VBO and load Data
  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_CELLS]);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(VertexPosColor)*cell_vis_vbo.size(),
               &cell_vis_vbo[0], GL_STATIC_DRAW);

  // To feed data for shaders?
  // glUniform1i(GLCanvas::colormodeID, 1);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,sizeof(VertexPosColor),0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(VertexPosColor),(GLvoid*)sizeof(glm::vec4) );

  HandleGLError("exit setupCellVis");

}

void ParticleSystem::drawCellsVis() {
  HandleGLError("enter drawCellsVis");

  glBindVertexArray(VaoId[GL_CELLS]);
  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_CELLS]);

  glDrawArrays(GL_TRIANGLES, 0, 6*6*args->gridDivisions*args->gridDivisions);

  HandleGLError("leaving drawCellsVis");

}

void ParticleSystem::setupCubeVBO(const glm::vec4 pts[8], const glm::vec4 &color, std::vector<VertexPosColor> &faces) {

  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[1],color));
  faces.push_back(VertexPosColor(pts[3],color));
  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[3],color));
  faces.push_back(VertexPosColor(pts[2],color));

  faces.push_back(VertexPosColor(pts[4],color));
  faces.push_back(VertexPosColor(pts[6],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[4],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[5],color));

  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[2],color));
  faces.push_back(VertexPosColor(pts[6],color));
  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[6],color));
  faces.push_back(VertexPosColor(pts[4],color));

  faces.push_back(VertexPosColor(pts[1],color));
  faces.push_back(VertexPosColor(pts[5],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[1],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[3],color));

  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[4],color));
  faces.push_back(VertexPosColor(pts[5],color));
  faces.push_back(VertexPosColor(pts[0],color));
  faces.push_back(VertexPosColor(pts[5],color));
  faces.push_back(VertexPosColor(pts[1],color));

  faces.push_back(VertexPosColor(pts[2],color));
  faces.push_back(VertexPosColor(pts[3],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[2],color));
  faces.push_back(VertexPosColor(pts[7],color));
  faces.push_back(VertexPosColor(pts[6],color));

}

// ╦ ╦╔═╗╦  ╦    ╦  ╦╔╗╔╔═╗╔═╗
// ║║║╠═╣║  ║    ║  ║║║║║╣ ╚═╗
// ╚╩╝╩ ╩╩═╝╩═╝  ╩═╝╩╝╚╝╚═╝╚═╝

void ParticleSystem::setupWalls() {

  HandleGLError("enter setupWalls");

  VertexPosColor* wallsVertex = new VertexPosColor[2*wallVec.size()];

  glm::vec4 white(1,1,1,1);
  unsigned int index = 0;
  unsigned int z = args->worldRange / 2.0;

  // For each wall put its a and b in the VBO
  for(unsigned int i = 0; i < wallVec.size(); i++){

      // Getting pos
      Vec3f a = wallVec[i]->getA();
      Vec3f b = wallVec[i]->getB();

      // seting for glm
      glm::vec4 aPos(a.x(),a.y(),z,1);
      glm::vec4 bPos(b.x(),b.y(),z,1);

      wallsVertex[index++] = VertexPosColor(aPos, white);
      wallsVertex[index++] = VertexPosColor(bPos, white);

  }

  assert(index == 2*wallVec.size());


  // Working  with Particles VAO
  glBindVertexArray(VaoId[GL_WALL]);

  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_WALL]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(VertexPosColor) * wallVec.size() * 2, wallsVertex ,GL_STATIC_DRAW);

  // For postion
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);

  // For color
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)sizeof(glm::vec4));


  delete [] wallsVertex;
  HandleGLError("leaving setupWalls");

}

void ParticleSystem::drawWalls() const {

  HandleGLError("enter drawWalls");

  glBindVertexArray(VaoId[GL_WALL]);
  glBindBuffer(GL_ARRAY_BUFFER,VboId[GL_WALL]);


  glDrawArrays(
              GL_LINES,    // The primitive you want to draw it as
              0,            // The start offset
              wallVec.size()*2);  // The number of points

  HandleGLError("leaving drawWalls");

}
