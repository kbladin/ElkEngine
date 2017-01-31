#pragma once

#include "sge/core/new_mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

namespace sge { namespace core {

class CreateMesh
{
public:
  CreateMesh() {};
  ~CreateMesh() {};
  
  static std::shared_ptr<NewMesh> load(const char* path);
  static std::shared_ptr<NewMesh> quad();
  static std::shared_ptr<NewMesh> box(glm::vec3 min, glm::vec3 max);
  static std::shared_ptr<NewMesh> cone(int segments);
  static std::shared_ptr<NewMesh> cylinder(int segments);
  static std::shared_ptr<NewMesh> lonLatSphere(int lon_segments, int lat_segments);
  static std::shared_ptr<NewMesh> line(glm::vec3 start, glm::vec3 end);
  static std::shared_ptr<NewMesh> grid(unsigned int segments);
  static std::shared_ptr<NewMesh> circle(unsigned int segments);
private:
  static std::pair<std::vector<unsigned short>, std::vector<glm::vec2>>
    createGridPlane(int s_segments, int t_segments);
};

} }
