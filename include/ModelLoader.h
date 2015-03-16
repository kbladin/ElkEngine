#include <vector>
#include <string>

#include "glm/glm.hpp"

class ModelLoader {
public:
	static bool load(
                   const char* file_name,
                   std::vector<glm::vec3>* vertices,
                   std::vector<glm::vec3>* normals,
                   std::vector<unsigned short>* elements);
};