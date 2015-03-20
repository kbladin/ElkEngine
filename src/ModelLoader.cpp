#include "ModelLoader.h"

#include "glm/glm.hpp"

#include <string>
#include <iostream>
#include <fstream>

bool ModelLoader::load(
                       const char* file_name,
                       std::vector<glm::vec3>* vertices,
                       std::vector<glm::vec3>* normals,
                       std::vector<unsigned short>* elements)
{
  if (std::strcmp(&file_name[std::strlen(file_name) - 2], ".m"))
  {
    std::cout << "Error: File format needs to be .m. Aborting." << std::endl;
    return false;
  }
  std::ifstream model_file(file_name);
  if (model_file.is_open())
  {
    std::cout << "File open: " << file_name << std::endl;
    std::string line;
    while ( getline (model_file,line) && line.substr(0,6) == "Vertex" )
    {
      line = line.substr(7); // Ignore "Vertex "
      std::string::size_type sz; // Alias of size_t
      std::stoi (line,&sz); // To ignore the vertex number
      
      line = line.substr(sz);
      float vert_x = std::stof (line, &sz);
      line = line.substr(sz);
      float vert_y = std::stof (line, &sz);
      line = line.substr(sz);
      float vert_z = std::stof (line, &sz);
      vertices->push_back(glm::vec3(vert_x, vert_y, vert_z));
      
      sz += 10; // Skip " {normal=("
      line = line.substr(sz);
      float normal_x = std::stof (line, &sz);
      line = line.substr(sz);
      float normal_y = std::stof (line, &sz);
      line = line.substr(sz);
      float normal_z = std::stof (line, &sz);
      normals->push_back(glm::vec3(normal_x, normal_y, normal_z));
    }
    while ( getline (model_file,line) && line.substr(0,4) == "Face" )
    {
      line = line.substr(5); // Ignore "Face "
      std::string::size_type sz; // Alias of size_t
      std::stoi (line,&sz); // To ignore the vertex number
      
      line = line.substr(sz);
      int vert_index_0 = std::stof (line, &sz) - 1;
      line = line.substr(sz);
      int vert_index_1 = std::stof (line, &sz) - 1;
      line = line.substr(sz);
      int vert_index_2 = std::stof (line, &sz) - 1;
      elements->push_back(vert_index_0);
      elements->push_back(vert_index_1);
      elements->push_back(vert_index_2);
    }
    model_file.close();
    std::cout << "File loaded: " << file_name << std::endl;
  }
  else
  {
    std::cout << "Error: Could not find file:" << file_name << std::endl;
    return false;
  }
  return true;
}