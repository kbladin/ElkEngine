#ifndef OBJECT_3D_H
#define OBJECT_3D_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//! An object positioned in 3D space.
/*!
  This object can not be rendered by itself. It needs a child or children in
  form of meshes (TriangleMesh or LineMesh).
  All objects inheriting from Object3D can be added as child.
*/
class Object3D {
public:
  Object3D() {};
  virtual ~Object3D();
  void addChild(Object3D* child);
  void removeChild(Object3D* child);
  virtual void render(glm::mat4 M);
  glm::mat4 transform_matrix_;
private:
  std::vector<Object3D*> children;
};

#endif