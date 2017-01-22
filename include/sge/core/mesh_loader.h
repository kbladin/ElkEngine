#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace sge { namespace core {

bool loadMesh_assimp(
  const char* 					path,
  std::vector<unsigned short>* 	out_indices,
  std::vector<glm::vec3>* 		out_vertices, 
  std::vector<glm::vec2>* 		out_uvs, 
  std::vector<glm::vec3>* 		out_normals);

} }