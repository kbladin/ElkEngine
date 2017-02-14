#pragma once

#include "elk/core/mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

namespace elk { namespace core {

class CreateMesh
{
public:
  CreateMesh() {};
  ~CreateMesh() {};
  
  static std::shared_ptr<Mesh> load(const char* path);
  static std::shared_ptr<Mesh> quad();
  static std::shared_ptr<Mesh> box(glm::vec3 min, glm::vec3 max);
  static std::shared_ptr<Mesh> cone(int segments);
  static std::shared_ptr<Mesh> cylinder(int segments);
  static std::shared_ptr<Mesh> lonLatSphere(int lon_segments, int lat_segments);
  static std::shared_ptr<Mesh> line(glm::vec3 start, glm::vec3 end);
  static std::shared_ptr<Mesh> grid(unsigned int segments);
  static std::shared_ptr<Mesh> circle(unsigned int segments);
private:
  static std::pair<std::vector<unsigned short>, std::vector<glm::vec2>>
    createGridPlane(int s_segments, int t_segments);
};

} }
