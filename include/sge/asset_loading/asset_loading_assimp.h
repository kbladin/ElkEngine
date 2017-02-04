#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace sge { namespace core {

//! Loads a mesh using the assimp library.
/*!
  \param path is a cstring of the path to the file.
  \param out_indices is the output indeces.
  \param out_uvs is the output uvs.
  \param out_normals is the output normals.
*/
bool loadMesh_assimp(
  const char* 					        path,
  std::vector<unsigned short>* 	out_indices,
  std::vector<glm::vec3>* 		  out_vertices, 
  std::vector<glm::vec2>* 		  out_uvs, 
  std::vector<glm::vec3>* 		  out_normals);

} }
