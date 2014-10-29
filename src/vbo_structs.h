#ifndef VBO_STRUCTS_H
#define VBO_STRUCTS_H


// ====================================================================
// some classes for VBOs to be used in my programs

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

#endif // VBO_STRUCTS_H
