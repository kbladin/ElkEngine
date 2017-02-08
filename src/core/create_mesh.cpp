#include "sge/core/create_mesh.h"

#ifdef SGE_USE_ASSIMP
  #include "sge/asset_loading/asset_loading_assimp.h"
#endif

namespace sge { namespace core {

std::shared_ptr<Mesh> CreateMesh::load(const char* path)
{
  std::shared_ptr<Mesh> result;

#ifdef SGE_USE_ASSIMP
  std::vector<unsigned short>* elements = new std::vector<unsigned short>;
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>;
  std::vector<glm::vec2>* texture_coordinates = new std::vector<glm::vec2>;
  std::vector<glm::vec3>* normals = new std::vector<glm::vec3>;

  if (!loadMesh_assimp(path, elements, positions, texture_coordinates, normals))
  {
    printf("ERROR : loading mesh failed\n");
    result = nullptr;
  }
  else
  {
    // Mesh takes ownership of the data!
    result = std::make_shared<Mesh>(elements, positions, normals, texture_coordinates);  
  }
#else
  printf("ERROR : Unable to read mesh without Assimp library\n");
  result = nullptr;
#endif

  return result;
}

std::shared_ptr<Mesh> CreateMesh::quad()
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>(6);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>(4);
  std::vector<glm::vec3>* normals = new std::vector<glm::vec3>(4);
  std::vector<glm::vec2>* texture_coordinates = new std::vector<glm::vec2>(4);
  
  // Create a quad
  (*positions)[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
  (*positions)[1] = glm::vec3(1.0f, -1.0f, 0.0f);
  (*positions)[2] = glm::vec3(-1.0f, 1.0f, 0.0f);
  (*positions)[3] = glm::vec3(1.0f, 1.0f, 0.0f);
  
  // Normals
  (*normals)[0] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[1] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[2] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[3] = glm::vec3(0.0f, 0.0f, 1.0f);

  // Normals
  (*texture_coordinates)[0] = glm::vec2(0.0f, 0.0f);
  (*texture_coordinates)[1] = glm::vec2(1.0f, 0.0f);
  (*texture_coordinates)[2] = glm::vec2(0.0f, 1.0f);
  (*texture_coordinates)[3] = glm::vec2(1.0f, 1.0f);

  // Connectivity info
  (*elements)[0] = 0;
  (*elements)[1] = 1;
  (*elements)[2] = 3;
  (*elements)[3] = 0;
  (*elements)[4] = 3;
  (*elements)[5] = 2;

  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(elements, positions, normals, texture_coordinates);
}

 std::shared_ptr<Mesh> CreateMesh::box(glm::vec3 min, glm::vec3 max)
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>(36);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>(24);
  std::vector<glm::vec3>* normals = new std::vector<glm::vec3>(24);
  
  // Bottom
  (*positions)[0] = glm::vec3(min.x, min.y, min.z);
  (*positions)[1] = glm::vec3(max.x, min.y, min.z);
  (*positions)[2] = glm::vec3(max.x, min.y, max.z);
  (*positions)[3] = glm::vec3(min.x, min.y, max.z);
  // Left
  (*positions)[4] = glm::vec3(min.x, max.y, max.z);
  (*positions)[5] = glm::vec3(min.x, max.y, min.z);
  (*positions)[6] = glm::vec3(min.x, min.y, min.z);
  (*positions)[7] = glm::vec3(min.x, min.y, max.z);
  // Back
  (*positions)[8] = glm::vec3(min.x, min.y, min.z);
  (*positions)[9] = glm::vec3(min.x, max.y, min.z);
  (*positions)[10] = glm::vec3(max.x, max.y, min.z);
  (*positions)[11] = glm::vec3(max.x, min.y, min.z);
  // Right
  (*positions)[12] = glm::vec3(max.x, min.y, min.z);
  (*positions)[13] = glm::vec3(max.x, max.y, min.z);
  (*positions)[14] = glm::vec3(max.x, max.y, max.z);
  (*positions)[15] = glm::vec3(max.x, min.y, max.z);
  // Front
  (*positions)[16] = glm::vec3(max.x, min.y, max.z);
  (*positions)[17] = glm::vec3(max.x, max.y, max.z);
  (*positions)[18] = glm::vec3(min.x, max.y, max.z);
  (*positions)[19] = glm::vec3(min.x, min.y, max.z);
  // Top
  (*positions)[20] = glm::vec3(min.x, max.y, min.z);
  (*positions)[21] = glm::vec3(min.x, max.y, max.z);
  (*positions)[22] = glm::vec3(max.x, max.y, max.z);
  (*positions)[23] = glm::vec3(max.x, max.y, min.z);
  
  // Bottom
  (*normals)[0] = glm::vec3(0.0f, -1.0f, 0.0f);
  (*normals)[1] = glm::vec3(0.0f, -1.0f, 0.0f);
  (*normals)[2] = glm::vec3(0.0f, -1.0f, 0.0f);
  (*normals)[3] = glm::vec3(0.0f, -1.0f, 0.0f);
  // Left
  (*normals)[4] = glm::vec3(-1.0f, 0.0f, 0.0f);
  (*normals)[5] = glm::vec3(-1.0f, 0.0f, 0.0f);
  (*normals)[6] = glm::vec3(-1.0f, 0.0f, 0.0f);
  (*normals)[7] = glm::vec3(-1.0f, 0.0f, 0.0f);
  // Back
  (*normals)[8] = glm::vec3(0.0f, 0.0f, -1.0f);
  (*normals)[9] = glm::vec3(0.0f, 0.0f, -1.0f);
  (*normals)[10] = glm::vec3(0.0f, 0.0f, -1.0f);
  (*normals)[11] = glm::vec3(0.0f, 0.0f, -1.0f);
  // Right
  (*normals)[12] = glm::vec3(1.0f, 0.0f, 0.0f);
  (*normals)[13] = glm::vec3(1.0f, 0.0f, 0.0f);
  (*normals)[14] = glm::vec3(1.0f, 0.0f, 0.0f);
  (*normals)[15] = glm::vec3(1.0f, 0.0f, 0.0f);
  // Front
  (*normals)[16] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[17] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[18] = glm::vec3(0.0f, 0.0f, 1.0f);
  (*normals)[19] = glm::vec3(0.0f, 0.0f, 1.0f);
  // Top
  (*normals)[20] = glm::vec3(0.0f, 1.0f, 0.0f);
  (*normals)[21] = glm::vec3(0.0f, 1.0f, 0.0f);
  (*normals)[22] = glm::vec3(0.0f, 1.0f, 0.0f);
  (*normals)[23] = glm::vec3(0.0f, 1.0f, 0.0f);

  // Bottom
  (*elements)[0] = 0;
  (*elements)[1] = 1;
  (*elements)[2] = 2;
  
  (*elements)[3] = 2;
  (*elements)[4] = 3;
  (*elements)[5] = 0;
  // Left
  (*elements)[6] = 4;
  (*elements)[7] = 5;
  (*elements)[8] = 6;
  
  (*elements)[9] = 6;
  (*elements)[10] = 7;
  (*elements)[11] = 4;
  // Back
  (*elements)[12] = 8;
  (*elements)[13] = 9;
  (*elements)[14] = 10;
  
  (*elements)[15] = 10;
  (*elements)[16] = 11;
  (*elements)[17] = 8;
  // Right
  (*elements)[18] = 12;
  (*elements)[19] = 13;
  (*elements)[20] = 14;
  
  (*elements)[21] = 14;
  (*elements)[22] = 15;
  (*elements)[23] = 12;
  // Front
  (*elements)[24] = 16;
  (*elements)[25] = 17;
  (*elements)[26] = 18;
  
  (*elements)[27] = 18;
  (*elements)[28] = 19;
  (*elements)[29] = 16;
  // Top
  (*elements)[30] = 20;
  (*elements)[31] = 21;
  (*elements)[32] = 22;
  
  (*elements)[33] = 22;
  (*elements)[34] = 23;
  (*elements)[35] = 20;
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(elements, positions, normals);
}

 std::shared_ptr<Mesh> CreateMesh::cone(int segments)
{  
  std::vector<unsigned short>* elements = new std::vector<unsigned short>(segments * 6);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>(segments + 2);
  std::vector<glm::vec3>* normals = new std::vector<glm::vec3>(segments + 2);

  // Vertices around the base of the cone
  float delta_theta = M_PI * 2 / float(segments);
  for (int i=0; i<segments; i++) {
    float x = cos(delta_theta * i);
    float z = sin(delta_theta * i);
    (*positions)[i] = glm::vec3(x,0,z);
    (*normals)[i] = glm::vec3(x,0,z);
  }

  // Top and bottom vertices
  (*positions)[segments] = glm::vec3(0,1,0);
  (*normals)[segments] = glm::vec3(0,1,0);
  (*positions)[segments + 1] = glm::vec3(0,0,0);
  (*normals)[segments + 1] = glm::vec3(0,-1,0);
  
  // Set connectivity
  for (int i = 0; i<segments-1; i++) {
    (*elements)[i*6] = i;
    (*elements)[i*6 + 1] = segments;
    (*elements)[i*6 + 2] = i + 1;
    (*elements)[i*6 + 3] = i;
    (*elements)[i*6 + 4] = i + 1;
    (*elements)[i*6 + 5] = segments + 1;
  }
  (*elements)[segments*6 - 1 - 5] = segments - 1;
  (*elements)[segments*6 - 1 - 4] = segments;
  (*elements)[segments*6 - 1 - 3] = 0;
  (*elements)[segments*6 - 1 - 2] = segments - 1;
  (*elements)[segments*6 - 1 - 1] = 0;
  (*elements)[segments*6 - 1 - 0] = segments + 1;
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(elements, positions, normals);
}

 std::shared_ptr<Mesh> CreateMesh::cylinder(int segments)
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>(segments * 12);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>(segments * 2 + (segments + 1) * 2);
  std::vector<glm::vec3>* normals = new std::vector<glm::vec3>(segments * 2 + (segments + 1) * 2);

  float delta_theta = M_PI * 2 / float(segments);
  for (int i = 0; i<segments; i++) {
    float x = cos(delta_theta * i);
    float z = -sin(delta_theta * i);
    
    (*positions)[i] = glm::vec3(x,1,z) * 0.5f;
    (*normals)[i] = glm::vec3(x,0,z);
    
    (*positions)[i + segments] = glm::vec3(x,-1,z) * 0.5f;
    (*normals)[i + segments] = glm::vec3(x,0,z);
    
    (*positions)[i + 2*segments] = glm::vec3(x,1,z) * 0.5f;
    (*normals)[i + 2*segments] = glm::vec3(0,1,0);
    
    (*positions)[i + 3*segments] = glm::vec3(x,-1,z) * 0.5f;
    (*normals)[i + 3*segments] = glm::vec3(0,-1,0);
  }
  (*positions)[positions->size() - 2] = glm::vec3(0,1,0) * 0.5f;
  (*normals)[normals->size() - 2] = glm::vec3(0,1,0);
  (*positions)[positions->size() - 1] = glm::vec3(0,-1,0) * 0.5f;
  (*normals)[normals->size() - 1] = glm::vec3(0,-1,0);
  
  for (int i = 0; i<segments-1; i++) {
    (*elements)[i*12] = i;
    (*elements)[i*12 + 1] = i + segments;
    (*elements)[i*12 + 2] = i + 1;
    
    (*elements)[i*12 + 3] = i + 1;
    (*elements)[i*12 + 4] = i + segments;
    (*elements)[i*12 + 5] = i + 1 + segments;

    (*elements)[i*12 + 6] = i + segments*2;
    (*elements)[i*12 + 7] = i + segments*2 + 1;
    (*elements)[i*12 + 8] = positions->size() - 2;
    
    (*elements)[i*12 + 9] = i + segments*2 + 1 + segments;
    (*elements)[i*12 + 10] = i + segments*2 + segments;
    (*elements)[i*12 + 11] = positions->size() - 1;
  }
  
  (*elements)[segments*12 - 1 - 11] = segments - 1;
  (*elements)[segments*12 - 1 - 10] = segments - 1 + segments;
  (*elements)[segments*12 - 1 - 9] = 0;
  
  
  (*elements)[segments*12 - 1 - 8] = 0;
  (*elements)[segments*12 - 1 - 7] = segments - 1 + segments;
  (*elements)[segments*12 - 1 - 6] = segments;
  
  (*elements)[segments*12 - 1 - 5] = segments*2 + segments - 1;
  (*elements)[segments*12 - 1 - 4] = segments*2 + 0;
  (*elements)[segments*12 - 1 - 3] = positions->size() - 2;
  
  (*elements)[segments*12 - 1 - 2] = segments*2 + segments;
  (*elements)[segments*12 - 1 - 1] = segments*2 + segments - 1 + segments;
  (*elements)[segments*12 - 1 - 0] = positions->size() - 1;
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(elements, positions, normals);
}

std::shared_ptr<Mesh> CreateMesh::lonLatSphere(int lon_segments, int lat_segments)
{
  auto grid_plane = createGridPlane(lon_segments, lat_segments);
  std::vector<unsigned short>* elements =       
    new std::vector<unsigned short>(grid_plane.first);
  std::vector<glm::vec3>* positions =           
    new std::vector<glm::vec3>((lon_segments + 1) * (lat_segments + 1));
  std::vector<glm::vec3>* normals =             
    new std::vector<glm::vec3>((lon_segments + 1) * (lat_segments + 1));
  std::vector<glm::vec2>* texture_coordinates = 
    new std::vector<glm::vec2>((lon_segments + 1) * (lat_segments + 1));
  std::vector<glm::vec3>* tangents =
    new std::vector<glm::vec3>((lon_segments + 1) * (lat_segments + 1));

  for (int i = 0; i < grid_plane.second.size(); ++i)
  {
    float theta = (grid_plane.second[i].x - 0.5) * 2 * M_PI;
    float phi = (grid_plane.second[i].y - 0.5) * M_PI;
    
    float x = cos(phi) * cos(theta);
    float y = sin(phi);
    float z = -sin(theta) * cos(phi);

    glm::vec3 n = normalize(glm::vec3(x, y, z));
    glm::vec3 y_vec = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 tangent = glm::cross(y_vec, n);
    
    (*positions)[i] = glm::vec3(x, y, z);
    (*normals)[i] = n;
    (*texture_coordinates)[i] = grid_plane.second[i];
    (*tangents)[i] = tangent;
  }

  return std::make_shared<Mesh>(
    elements, positions, normals, texture_coordinates, tangents);
}

std::shared_ptr<Mesh> CreateMesh::line(glm::vec3 start, glm::vec3 end)
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>;
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>;
  
  positions->push_back(start);
  positions->push_back(end);
  
  elements->push_back(0);
  elements->push_back(1);
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(
    elements, positions, nullptr, nullptr, nullptr, nullptr, GL_LINES);
}

 std::shared_ptr<Mesh> CreateMesh::grid(unsigned int segments)
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>((segments + 1) * 4);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>((segments + 1) * 4);

  int i = 0;
  for (int j=0; i < segments + 1; i++, j++) {
    (*positions)[i] = glm::vec3(j,0.0f,0.0f) / static_cast<float>(segments) - glm::vec3(0.5f,0.5f,0.0f);
  }
  for (int j=0; i < (segments + 1) * 2; i++, j++) {
    (*positions)[i] = glm::vec3(0.0f,j,0.0f) / static_cast<float>(segments) - glm::vec3(0.5f,0.5f,0.0f);
  }
  for (int j=0; i < (segments + 1) * 3; i++, j++) {
    (*positions)[i] = glm::vec3(j,segments,0.0f) / static_cast<float>(segments) - glm::vec3(0.5f,0.5f,0.0f);
  }
  for (int j=0; i < (segments + 1) * 4; i++, j++) {
    (*positions)[i] = glm::vec3(segments,j,0.0f) / static_cast<float>(segments) - glm::vec3(0.5f,0.5f,0.0f);
  }
  
  for (int i = 0; i < elements->size(); i = i+2) {
    (*elements)[i] = i/2;
  }
  for (int i = 1; i < elements->size(); i = i+2) {
    (*elements)[i] = i/2 + (segments + 1) * 2;
  }
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(
    elements, positions, nullptr, nullptr, nullptr, nullptr, GL_LINES);
}

 std::shared_ptr<Mesh> CreateMesh::circle(unsigned int segments)
{
  std::vector<unsigned short>* elements = new std::vector<unsigned short>(segments * 2);
  std::vector<glm::vec3>* positions = new std::vector<glm::vec3>(segments);
  
  float delta_theta = M_PI * 2 / float(segments);
  for (int i = 0; i<segments; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    (*positions)[i] = glm::vec3(x,y,0) * 0.5f;
    (*elements)[i*2] = i;
    (*elements)[i*2 + 1] = i + 1;
  }
  (*elements)[segments*2 - 1] = 0;
  
  // Mesh takes ownership of the data!
  return std::make_shared<Mesh>(
    elements, positions, nullptr, nullptr, nullptr, nullptr, GL_LINES);
}

std::pair<std::vector<unsigned short>, std::vector<glm::vec2>>
  CreateMesh::createGridPlane(int s_segments, int t_segments)
{
  std::vector<glm::vec2> st_coords((s_segments + 1) * (t_segments + 1));
  std::vector<unsigned short> elements(s_segments * t_segments * 6);
  for (int t = 0; t < t_segments + 1; ++t)
  {
    for (int s = 0; s < s_segments + 1; ++s)
    {
      int index = s + (s_segments + 1) * t;
      st_coords[index] = glm::vec2(static_cast<float>(s) / s_segments, static_cast<float>(t) / t_segments);
    }
  }

  for (int t = 0; t < t_segments; ++t)
  {
    for (int s = 0; s < s_segments; ++s)
    {
      int index = (s + s_segments * t) * 6;
      int element = s + (s_segments + 1) * t;
      elements[index + 0] = element;
      elements[index + 1] = element + 1;
      elements[index + 2] = element + s_segments + 2;
      elements[index + 3] = element;
      elements[index + 4] = element + s_segments + 2;
      elements[index + 5] = element + s_segments + 1;
    }
  }
  return {elements, st_coords};
}

} }
