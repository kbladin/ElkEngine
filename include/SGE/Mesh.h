#ifndef MESH_H
#define MESH_H

#include "SGE/MeshLoader.h"

#include <vector>

#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//! This class serves as a base for the mesh classes
/*!
  All meshes have a list of vertices but depending of type it can have different
  connectivity information.
*/
class AbstractMesh {
public:
  AbstractMesh();
  ~AbstractMesh();

  virtual void render() = 0;
  glm::vec3 computeMinPosition() const;
  glm::vec3 computeMaxPosition() const;
  
protected:
  virtual void initialize() = 0;

  // OpenGL handles  
  GLuint _vertex_array_ID;
  GLuint _vertex_buffer;

  // Data
  std::vector<glm::vec3> _vertices;
};

//! This class extends AbstractMesh and renders triangles
  /*!
   The TriangleMesh also has a list of normals and elements.
  */
class TriangleMesh : public AbstractMesh{
public:
  TriangleMesh(const char *file_name);
  TriangleMesh(
    std::vector<glm::vec3> vertices,
    std::vector<glm::vec3> normals,
    std::vector<unsigned short> elements);
  TriangleMesh();
  ~TriangleMesh();
  
  // Initialization functions
  void initPlane(
    glm::vec3 normal,
    glm::vec3 scale);
  void initBox(
    glm::vec3 max,
    glm::vec3 min);
  void initCone(
    glm::vec3 direction,
    glm::vec3 scale,
    int divisions);
  void initCylinder(
    glm::vec3 direction,
    glm::vec3 scale,
    int divisions);

  virtual void render();
private:
  void initialize();

  // OpenGL handles
  GLuint _element_buffer;
  GLuint _normal_buffer;

  // Data
  // Maximum around 60000 vertices for unsigned short.
  std::vector<unsigned short> _elements;
  std::vector<glm::vec3>      _normals;
};

//! This class extends AbstractMesh and renders lines
/*!
  Currently, this mesh can not be loaded from file.
*/
class LineMesh : public AbstractMesh {
public:
  LineMesh();
  ~LineMesh();

  // Initialization functions
  void initLine(
    glm::vec3 start,
    glm::vec3 end);
  void initGridPlane(
    glm::vec3 normal,
    glm::vec3 scale,
    unsigned int divisions);
  void initCircle(
    glm::vec3 normal,
    glm::vec3 scale,
    unsigned int divisions);
  
  virtual void render();
private:
  void initialize();

  // OpenGL handles
  GLuint _element_buffer;

  // Data
  // Maximum around 60000 vertices for unsigned short.
  std::vector<unsigned short> _elements;
};

//! This class extends AbstractMesh
class GPUPointCloudMesh : public AbstractMesh {
public:
  GPUPointCloudMesh(int size);
  ~GPUPointCloudMesh();

  virtual void render();
private:
  void initialize();
  
  const int size_;
  GLuint _index_buffer;
};

//! This class extends AbstractMesh
class CPUPointCloudMesh : public AbstractMesh{
public:
  CPUPointCloudMesh();
  ~CPUPointCloudMesh();

  void update(const std::vector<glm::vec3>& vertices);
  virtual void render();
protected:
  virtual void initialize();
};

#endif