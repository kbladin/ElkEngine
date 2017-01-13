#include "SGE/Mesh.h"

AbstractMesh::AbstractMesh()
{
}

AbstractMesh::~AbstractMesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &_vertex_buffer);
  glDeleteVertexArrays(1, &_vertex_array_ID);
}

void AbstractMesh::initialize()
{
  glGenVertexArrays(1, &_vertex_array_ID);
  glBindVertexArray(_vertex_array_ID);
  
  glGenBuffers(1, &_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    _vertices.size() * sizeof(glm::vec3),
    &_vertices[0],
    GL_STATIC_DRAW);
}

//! Create a TriangleMesh from file.
/*!
  \param file_name is the file path for the model, for example "bunny.obj".
  \param material is the material of the mesh
*/
TriangleMesh::TriangleMesh(const char *file_name)
{
  loadMesh_assimp(file_name, &_elements, &_vertices, NULL, &_normals);
  initialize();
}

//! Create a TriangleMesh from vertex lists.
/*!
  \param vertices is a list of vertices.
  \param normals is a list of normals.
  \param elements is a list of elements (indices for faces).
  \param material is the material of the mesh
  defined in SimpleGraphiicsEngine.
*/
TriangleMesh::TriangleMesh(
  std::vector<glm::vec3> vertices,
  std::vector<glm::vec3> normals,
  std::vector<unsigned short> elements)
{
  _vertices = vertices;
  _normals = normals;
  _elements = elements;
  
  initialize();
}

//! Creates a TriangleMesh without initializing vertex lists.
/*!
  This constructor is used when creating basic TriangleMeshes such as planes
  or boxes.
  First construct the TriangleMesh, then call one of the init-functions
  (eg: initPlane, initBox).
  \param material is the material of the mesh
*/
TriangleMesh::TriangleMesh()
{
}

TriangleMesh::~TriangleMesh()
{
  glDeleteBuffers(1, &_element_buffer);
  glDeleteBuffers(1, &_normal_buffer);
}

//! Initializes a plane.
/*!
  \param position is a point in the center of the plane.
  \param normal is the normal of the plane.
  \param scale defines scaling in x-, y-, and z- direction.
*/
void TriangleMesh::initPlane(
  glm::vec3 normal,
  glm::vec3 scale)
{
  // Data sizes
  _vertices.resize(4);
  _normals.resize(4);
  _elements.resize(6);
  
  // Create a quad
  _vertices[0] = glm::vec3(0.5f, 0.5f, 0.0f);
  _vertices[1] = glm::vec3(0.5f, -0.5f, 0.0f);
  _vertices[2] = glm::vec3(-0.5f, -0.5f, 0.0f);
  _vertices[3] = glm::vec3(-0.5f, 0.5f, 0.0f);
  
  // Connectivity info
  _elements[0] = 0;
  _elements[1] = 1;
  _elements[2] = 2;
  _elements[3] = 2;
  _elements[4] = 3;
  _elements[5] = 0;
  
  // Transform the vertices
  glm::mat4 M;
  glm::vec3 up =
    normal != glm::vec3(0.0f, 0.0f, 1.0f) ?
    glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 0.0f),
    normal,
    up);
  for (int i = 0; i < _vertices.size(); i++) {
    _normals[i] = glm::vec3(glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0) * M);
    _vertices[i] = glm::vec3(glm::vec4(_vertices[i] * scale, 1) * M);
  }

  initialize();
}

//! Initializes a axis aligned box.
/*!
  \param max is the max corner of the box
  \param min is the min corner of the box.
  \param position is a point in the center of the box.
*/
void TriangleMesh::initBox(glm::vec3 max, glm::vec3 min)
{
  // Data sizes
  _vertices.resize(24);
  _normals.resize(24);
  _elements.resize(36);
  
  // Bottom
  _vertices[0] = glm::vec3(min.x, min.y, min.z);
  _vertices[1] = glm::vec3(max.x, min.y, min.z);
  _vertices[2] = glm::vec3(max.x, min.y, max.z);
  _vertices[3] = glm::vec3(min.x, min.y, max.z);
  // Left
  _vertices[4] = glm::vec3(min.x, max.y, max.z);
  _vertices[5] = glm::vec3(min.x, max.y, min.z);
  _vertices[6] = glm::vec3(min.x, min.y, min.z);
  _vertices[7] = glm::vec3(min.x, min.y, max.z);
  // Back
  _vertices[8] = glm::vec3(min.x, min.y, min.z);
  _vertices[9] = glm::vec3(min.x, max.y, min.z);
  _vertices[10] = glm::vec3(max.x, max.y, min.z);
  _vertices[11] = glm::vec3(max.x, min.y, min.z);
  // Right
  _vertices[12] = glm::vec3(max.x, min.y, min.z);
  _vertices[13] = glm::vec3(max.x, max.y, min.z);
  _vertices[14] = glm::vec3(max.x, max.y, max.z);
  _vertices[15] = glm::vec3(max.x, min.y, max.z);
  // Front
  _vertices[16] = glm::vec3(max.x, min.y, max.z);
  _vertices[17] = glm::vec3(max.x, max.y, max.z);
  _vertices[18] = glm::vec3(min.x, max.y, max.z);
  _vertices[19] = glm::vec3(min.x, min.y, max.z);
  // Top
  _vertices[20] = glm::vec3(min.x, max.y, min.z);
  _vertices[21] = glm::vec3(min.x, max.y, max.z);
  _vertices[22] = glm::vec3(max.x, max.y, max.z);
  _vertices[23] = glm::vec3(max.x, max.y, min.z);
  
  // Bottom
  _normals[0] = glm::vec3(0.0f, -1.0f, 0.0f);
  _normals[1] = glm::vec3(0.0f, -1.0f, 0.0f);
  _normals[2] = glm::vec3(0.0f, -1.0f, 0.0f);
  _normals[3] = glm::vec3(0.0f, -1.0f, 0.0f);
  // Left
  _normals[4] = glm::vec3(-1.0f, 0.0f, 0.0f);
  _normals[5] = glm::vec3(-1.0f, 0.0f, 0.0f);
  _normals[6] = glm::vec3(-1.0f, 0.0f, 0.0f);
  _normals[7] = glm::vec3(-1.0f, 0.0f, 0.0f);
  // Back
  _normals[8] = glm::vec3(0.0f, 0.0f, -1.0f);
  _normals[9] = glm::vec3(0.0f, 0.0f, -1.0f);
  _normals[10] = glm::vec3(0.0f, 0.0f, -1.0f);
  _normals[11] = glm::vec3(0.0f, 0.0f, -1.0f);
  // Right
  _normals[12] = glm::vec3(1.0f, 0.0f, 0.0f);
  _normals[13] = glm::vec3(1.0f, 0.0f, 0.0f);
  _normals[14] = glm::vec3(1.0f, 0.0f, 0.0f);
  _normals[15] = glm::vec3(1.0f, 0.0f, 0.0f);
  // Front
  _normals[16] = glm::vec3(0.0f, 0.0f, 1.0f);
  _normals[17] = glm::vec3(0.0f, 0.0f, 1.0f);
  _normals[18] = glm::vec3(0.0f, 0.0f, 1.0f);
  _normals[19] = glm::vec3(0.0f, 0.0f, 1.0f);
  // Top
  _normals[20] = glm::vec3(0.0f, 1.0f, 0.0f);
  _normals[21] = glm::vec3(0.0f, 1.0f, 0.0f);
  _normals[22] = glm::vec3(0.0f, 1.0f, 0.0f);
  _normals[23] = glm::vec3(0.0f, 1.0f, 0.0f);

  // Bottom
  _elements[0] = 0;
  _elements[1] = 1;
  _elements[2] = 2;
  
  _elements[3] = 2;
  _elements[4] = 3;
  _elements[5] = 0;
  // Left
  _elements[6] = 4;
  _elements[7] = 5;
  _elements[8] = 6;
  
  _elements[9] = 6;
  _elements[10] = 7;
  _elements[11] = 4;
  // Back
  _elements[12] = 8;
  _elements[13] = 9;
  _elements[14] = 10;
  
  _elements[15] = 10;
  _elements[16] = 11;
  _elements[17] = 8;
  // Right
  _elements[18] = 12;
  _elements[19] = 13;
  _elements[20] = 14;
  
  _elements[21] = 14;
  _elements[22] = 15;
  _elements[23] = 12;
  // Front
  _elements[24] = 16;
  _elements[25] = 17;
  _elements[26] = 18;
  
  _elements[27] = 18;
  _elements[28] = 19;
  _elements[29] = 16;
  // Top
  _elements[30] = 20;
  _elements[31] = 21;
  _elements[32] = 22;
  
  _elements[33] = 22;
  _elements[34] = 23;
  _elements[35] = 20;
  
  initialize();
}

//! Initializes a cone shape.
/*!
  Currently, the normals are all soft (not realistic).
  \param position is the position of the center of the circle
  \param direction is direction of the cone.
  \param scale defines scaling in x-, y-, and z- direction.
  \param divisions is the subdivision level.
*/
void TriangleMesh::initCone(
              glm::vec3 direction,
              glm::vec3 scale,
              int divisions)
{
  // Data sizes
  _vertices.resize(divisions + 2);
  _normals.resize(divisions + 2);
  _elements.resize(divisions * 6);

  // Vertices around the base of the cone
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i=0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    _vertices[i] = glm::vec3(x,y,0);
    _normals[i] = glm::vec3(x,y,0);
  }

  // Top and bottom vertices
  _vertices[divisions] = glm::vec3(0,0,-1);
  _normals[divisions] = glm::vec3(0,0,-1);
  _vertices[divisions + 1] = glm::vec3(0,0,0);
  _normals[divisions + 1] = glm::vec3(0,0,1);
  
  // Set connectivity
  for (int i = 0; i<divisions-1; i++) {
    _elements[i*6] = i;
    _elements[i*6 + 1] = divisions;
    _elements[i*6 + 2] = i + 1;
    _elements[i*6 + 3] = i;
    _elements[i*6 + 4] = i + 1;
    _elements[i*6 + 5] = divisions + 1;
  }
  _elements[divisions*6 - 1 - 5] = divisions - 1;
  _elements[divisions*6 - 1 - 4] = divisions;
  _elements[divisions*6 - 1 - 3] = 0;
  _elements[divisions*6 - 1 - 2] = divisions - 1;
  _elements[divisions*6 - 1 - 1] = 0;
  _elements[divisions*6 - 1 - 0] = divisions + 1;
  
  // Set direction
  glm::mat4 M;
  glm::vec3 up =
  !(direction.x == 0 && direction.y == 0) ?
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  direction,
                  up);

  // Apply direction transform
  for (int i = 0; i < _vertices.size(); i++) {
    _vertices[i].x *= scale.x;
    _vertices[i].y *= scale.y;
    _vertices[i].z *= scale.z; // Doing only this will make normals wrong
    _normals[i] = glm::vec3(glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0) * M);
    _vertices[i] = glm::vec3(glm::vec4(_vertices[i], 1) * M);
  }
  
  initialize();
}

//! Initializes a cylinder shape.
/*!
  \param position is the position of the center of the circle
  \param direction is direction of the cone.
  \param scale defines scaling in x-, y-, and z- direction.
  \param divisions is the subdivision level.
*/
void TriangleMesh::initCylinder(
                                glm::vec3 direction,
                                glm::vec3 scale,
                                int divisions)
{
  // Data sizes
  _vertices.resize(divisions * 2 + (divisions + 1) * 2);
  _normals.resize(divisions * 2 + (divisions + 1) * 2);
  _elements.resize(divisions * 12);
  
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i = 0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    
    _vertices[i] = glm::vec3(x,y,1) * 0.5f;
    _normals[i] = glm::vec3(x,y,0);
    
    _vertices[i + divisions] = glm::vec3(x,y,-1) * 0.5f;
    _normals[i + divisions] = glm::vec3(x,y,0);
    
    _vertices[i + 2*divisions] = glm::vec3(x,y,1) * 0.5f;
    _normals[i + 2*divisions] = glm::vec3(0,0,1);
    
    _vertices[i + 3*divisions] = glm::vec3(x,y,-1) * 0.5f;
    _normals[i + 3*divisions] = glm::vec3(0,0,-1);
  }
  _vertices[_vertices.size() - 2] = glm::vec3(0,0,1) * 0.5f;
  _normals[_normals.size() - 2] = glm::vec3(0,0,1);
  _vertices[_vertices.size() - 1] = glm::vec3(0,0,-1) * 0.5f;
  _normals[_normals.size() - 1] = glm::vec3(0,0,-1);
  
  for (int i = 0; i<divisions-1; i++) {
    _elements[i*12] = i;
    _elements[i*12 + 1] = i + divisions;
    _elements[i*12 + 2] = i + 1;
    
    _elements[i*12 + 3] = i + 1;
    _elements[i*12 + 4] = i + divisions;
    _elements[i*12 + 5] = i + 1 + divisions;

    _elements[i*12 + 6] = i + divisions*2;
    _elements[i*12 + 7] = i + divisions*2 + 1;
    _elements[i*12 + 8] = _vertices.size() - 2;
    
    _elements[i*12 + 9] = i + divisions*2 + 1 + divisions;
    _elements[i*12 + 10] = i + divisions*2 + divisions;
    _elements[i*12 + 11] = _vertices.size() - 1;
  }
  
  _elements[divisions*12 - 1 - 11] = divisions - 1;
  _elements[divisions*12 - 1 - 10] = divisions - 1 + divisions;
  _elements[divisions*12 - 1 - 9] = 0;
  
  
  _elements[divisions*12 - 1 - 8] = 0;
  _elements[divisions*12 - 1 - 7] = divisions - 1 + divisions;
  _elements[divisions*12 - 1 - 6] = divisions;
  
  _elements[divisions*12 - 1 - 5] = divisions*2 + divisions - 1;
  _elements[divisions*12 - 1 - 4] = divisions*2 + 0;
  _elements[divisions*12 - 1 - 3] = _vertices.size() - 2;
  
  _elements[divisions*12 - 1 - 2] = divisions*2 + divisions;
  _elements[divisions*12 - 1 - 1] = divisions*2 + divisions - 1 + divisions;
  _elements[divisions*12 - 1 - 0] = _vertices.size() - 1;
  
  glm::mat4 M;
  glm::vec3 up =
  !(direction.x == 0 && direction.y == 0) ?
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  direction,
                  up);
  
  for (int i = 0; i < _vertices.size(); i++) {
    _vertices[i].x *= scale.x;
    _vertices[i].y *= scale.y;
    _vertices[i].z *= scale.z; // Doing only this will make normals wrong
    _normals[i] = glm::vec3(glm::vec4(_normals[i], 0) * M);
    _vertices[i] = glm::vec3(glm::vec4(_vertices[i], 1) * M);
  }
  
  initialize();
}

void TriangleMesh::initialize()
{
  AbstractMesh::initialize();
  
  glGenBuffers(1, &_normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    _normals.size() * sizeof(glm::vec3),
    &_normals[0],
    GL_STATIC_DRAW);
  
  glGenBuffers(1, &_element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    _elements.size() * sizeof(unsigned short),
    &_elements[0],
    GL_STATIC_DRAW);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
*/
void TriangleMesh::render()
{ 
  glBindVertexArray(_vertex_array_ID);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
  glVertexAttribPointer(
    0,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _normal_buffer);
  glVertexAttribPointer(
    1,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
    
  // Draw the triangles
  glDrawElements(
    GL_TRIANGLES,       // mode
    _elements.size(),   // count
    GL_UNSIGNED_SHORT,  // type
    (void*)0);          // element array buffer offset
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

//! Creates a LineMesh without initializing vertex lists.
/*!
  This constructor is used when creating basic LineMeshes such as grid-planes
  or axes.
  First construct the LineMesh, then call one of the init-functions
  (eg: initAxes, initGridPlane).
*/
LineMesh::LineMesh()
{
  initialize();
}

LineMesh::~LineMesh()
{
  glDeleteBuffers(1, &_element_buffer);
}

//! Initializes line from start position to end position.
/*!
  \param start is the start position of the line.
  \param end is the end position of the line.
*/
void LineMesh::initLine(glm::vec3 start, glm::vec3 end)
{
  _vertices.push_back(start);
  _vertices.push_back(end);
  
  _elements.push_back(0);
  _elements.push_back(1);
  
  initialize();
}

//! Initializes a grid-plane used as reference in the scene.
/*!
  \param position is a point in the center of the plane.
  \param normal is the normal of the plane.
  \param scale defines scaling in x-, y-, and z- direction.
  \param divisions defines how dense the grid will be.
*/
void LineMesh::initGridPlane(
  glm::vec3 normal,
  glm::vec3 scale,
  unsigned int divisions)
{
  _vertices.resize((divisions + 1) * 4);
  _elements.resize((divisions + 1) * 4);
  int i = 0;
  for (int j=0; i < divisions + 1; i++, j++) {
    _vertices[i] = glm::vec3(j,0.0f,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 2; i++, j++) {
    _vertices[i] = glm::vec3(0.0f,j,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 3; i++, j++) {
    _vertices[i] = glm::vec3(j,divisions,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 4; i++, j++) {
    _vertices[i] = glm::vec3(divisions,j,0.0f);
  }
  
  for (int i = 0; i < _elements.size(); i = i+2) {
    _elements[i] = i/2;
  }
  for (int i = 1; i < _elements.size(); i = i+2) {
    _elements[i] = i/2 + (divisions + 1) * 2;
  }
  
  glm::mat4 M;
  glm::vec3 up =
  !(normal.x == 0 && normal.y == 0) ?
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 0.0f),
    normal,
    up);
    M *= glm::scale(scale);
  
  for (int i = 0; i < _vertices.size(); i++) {
    _vertices[i] /= divisions;
    _vertices[i] = glm::vec3(glm::vec4(_vertices[i], 1) * M);
  }
  
  initialize();
}

//! Initializes a circle.
/*!
  \param position is a point in the center of the circle.
  \param normal is the normal of the circle.
  \param scale defines scaling in x-, y-, and z- direction.
  \param divisions defines how many vertices the circle will contain.
*/
void LineMesh::initCircle(
                glm::vec3 normal,
                glm::vec3 scale,
                unsigned int divisions)
{
  _vertices.resize(divisions);
  _elements.resize(divisions*2);
  
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i = 0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    _vertices[i] = glm::vec3(x,y,0) * 0.5f;
    _elements[i*2] = i;
    _elements[i*2 + 1] = i + 1;
  }
  _elements[divisions*2 - 1] = 0;
  
  glm::mat4 M;
  glm::vec3 up =
    !(normal.x == 0 && normal.y == 0) ?
    glm::vec3(0.0f, 0.0f, 1.0f) :
    glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  normal,
                  up);
    M *= glm::scale(scale);

  for (int i = 0; i < _vertices.size(); i++) {
    //_vertices[i] /= divisions;
    _vertices[i] = glm::vec3(glm::vec4(_vertices[i], 1) * M);
  }
  
  initialize();
}

void LineMesh::initialize()
{
  AbstractMesh::initialize();
  
  glGenBuffers(1, &_element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    _elements.size() * sizeof(unsigned short),
    &_elements[0],
    GL_STATIC_DRAW);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
  \param program_ID is the shader that will be bound for rendering.
*/
void LineMesh::render()
{
  glBindVertexArray(_vertex_array_ID);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
  glVertexAttribPointer(
    0,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
  
  // Draw the triangles
  glDrawElements(
    GL_LINES,           // mode
    _elements.size(),   // count
    GL_UNSIGNED_SHORT,  // type
    (void*)0);          // element array buffer offset


  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

//! Creates a point cloud mesh of size * size particles 
GPUPointCloudMesh::GPUPointCloudMesh(int size) : size_(size)
{
  initialize();
}

void GPUPointCloudMesh::initialize()
{
  AbstractMesh::initialize();
  
  std::vector< std::pair<int, int> >  indices;
  indices.resize(size_ * size_);
  for (int i=0; i<size_; i++) {
    for (int j=0; j<size_; j++) {
      indices[i * size_ + j] = std::pair<int, int>(i,j);
    }
  }
  glGenBuffers(1, &_index_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, _index_buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    indices.size() * sizeof(std::pair<int, int>),
    &indices[0],
    GL_STATIC_DRAW);
}

//! Destructor
GPUPointCloudMesh::~GPUPointCloudMesh()
{
  glDeleteBuffers(1, &_index_buffer);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
  \param program_ID is the shader that will be bound for rendering.
*/
void GPUPointCloudMesh::render()
{
    
  glBindVertexArray(_vertex_array_ID);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _index_buffer);
  glVertexAttribPointer(
    0,          // attribute
    2,          // size
    GL_INT,     // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glDrawArrays(GL_POINTS, 0, size_ * size_);
  
  glDisableVertexAttribArray(0);
}


//! Creates a point cloud mesh of size * size particles 
CPUPointCloudMesh::CPUPointCloudMesh()
{
  initialize();
}

void CPUPointCloudMesh::initialize()
{
  AbstractMesh::initialize();
}

//! Destructor
CPUPointCloudMesh::~CPUPointCloudMesh()
{

}

void CPUPointCloudMesh::update(const std::vector<glm::vec3>& vertices)
{
  _vertices = vertices;
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    _vertices.size() * sizeof(glm::vec3),
    &_vertices[0],
    GL_STATIC_DRAW);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
  \param program_ID is the shader that will be bound for rendering.
*/
void CPUPointCloudMesh::render()
{
  glBindVertexArray(_vertex_array_ID);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
  glVertexAttribPointer(
    0,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glDrawArrays(GL_POINTS, 0, _vertices.size());
  
  glDisableVertexAttribArray(0);
}
