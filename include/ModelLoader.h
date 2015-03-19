#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <vector>
#include <string>

#include "glm/glm.hpp"

//! This class can load files of the format ".m" specified in the assignment.
class ModelLoader {
public:
  //! Creates vertex lists for the specified file.
  /*!
   \param file_name is the path to the ".m" file
   \param vertices is a list of vertices.
   \param normals is a list of normals.
   \param elements is a list of elements (indices for faces).
  */
	static bool load(
                   const char* file_name,
                   std::vector<glm::vec3>* vertices,
                   std::vector<glm::vec3>* normals,
                   std::vector<unsigned short>* elements);
};

#endif