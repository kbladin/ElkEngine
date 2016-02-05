#include "SGE/Mesh.h"

AbstractMesh::AbstractMesh()
{
}

AbstractMesh::~AbstractMesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
}

void AbstractMesh::initialize()
{
  glGenVertexArrays(1, &vertex_array_ID_);
  glBindVertexArray(vertex_array_ID_);
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(
    GL_ARRAY_BUFFER,
    vertices_.size() * sizeof(glm::vec3),
    &vertices_[0],
    GL_STATIC_DRAW);
}

//! Create a TriangleMesh from file.
/*!
  \param file_name is the file path for the model, for example "bunny.obj".
  \param material is the material of the mesh
*/
TriangleMesh::TriangleMesh(const char *file_name)
{
  loadMesh_assimp(file_name, &elements_, &vertices_, NULL, &normals_);
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
  vertices_ = vertices;
  normals_ = normals;
  elements_ = elements;
  
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
  glDeleteBuffers(1, &element_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
}

//! Initializes a plane.
/*!
  \param position is a point in the center of the plane.
  \param normal is the normal of the plane.
  \param scale defines scaling in x-, y-, and z- direction.
*/
void TriangleMesh::initPlane(
  glm::vec3 position,
  glm::vec3 normal,
  glm::vec3 scale)
{
  // Data sizes
  vertices_.resize(4);
  normals_.resize(4);
  elements_.resize(6);
  
  // Create a quad
  vertices_[0] = glm::vec3(0.5f, 0.5f, 0.0f);
  vertices_[1] = glm::vec3(0.5f, -0.5f, 0.0f);
  vertices_[2] = glm::vec3(-0.5f, -0.5f, 0.0f);
  vertices_[3] = glm::vec3(-0.5f, 0.5f, 0.0f);
  
  // Connectivity info
  elements_[0] = 0;
  elements_[1] = 1;
  elements_[2] = 2;
  elements_[3] = 2;
  elements_[4] = 3;
  elements_[5] = 0;
  
  // Transform the vertices
  glm::mat4 M;
  glm::vec3 up =
    normal != glm::vec3(0.0f, 0.0f, 1.0f) ?
    glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 0.0f),
    normal,
    up);
  for (int i = 0; i < vertices_.size(); i++) {
    normals_[i] = glm::vec3(glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0) * M);
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i] * scale, 1) * M);
  }

  transform_matrix_ *= glm::translate(position);

  initialize();
}

//! Initializes a axis aligned box.
/*!
  \param max is the max corner of the box
  \param min is the min corner of the box.
  \param position is a point in the center of the box.
*/
void TriangleMesh::initBox(glm::vec3 max,
                   glm::vec3 min,
                   glm::vec3 position)
{
  // Data sizes
  vertices_.resize(24);
  normals_.resize(24);
  elements_.resize(36);
  
  // Bottom
  vertices_[0] = glm::vec3(min.x, min.y, min.z);
  vertices_[1] = glm::vec3(max.x, min.y, min.z);
  vertices_[2] = glm::vec3(max.x, min.y, max.z);
  vertices_[3] = glm::vec3(min.x, min.y, max.z);
  // Left
  vertices_[4] = glm::vec3(min.x, max.y, max.z);
  vertices_[5] = glm::vec3(min.x, max.y, min.z);
  vertices_[6] = glm::vec3(min.x, min.y, min.z);
  vertices_[7] = glm::vec3(min.x, min.y, max.z);
  // Back
  vertices_[8] = glm::vec3(min.x, min.y, min.z);
  vertices_[9] = glm::vec3(min.x, max.y, min.z);
  vertices_[10] = glm::vec3(max.x, max.y, min.z);
  vertices_[11] = glm::vec3(max.x, min.y, min.z);
  // Right
  vertices_[12] = glm::vec3(max.x, min.y, min.z);
  vertices_[13] = glm::vec3(max.x, max.y, min.z);
  vertices_[14] = glm::vec3(max.x, max.y, max.z);
  vertices_[15] = glm::vec3(max.x, min.y, max.z);
  // Front
  vertices_[16] = glm::vec3(max.x, min.y, max.z);
  vertices_[17] = glm::vec3(max.x, max.y, max.z);
  vertices_[18] = glm::vec3(min.x, max.y, max.z);
  vertices_[19] = glm::vec3(min.x, min.y, max.z);
  // Top
  vertices_[20] = glm::vec3(min.x, max.y, min.z);
  vertices_[21] = glm::vec3(min.x, max.y, max.z);
  vertices_[22] = glm::vec3(max.x, max.y, max.z);
  vertices_[23] = glm::vec3(max.x, max.y, min.z);
  
  // Bottom
  normals_[0] = glm::vec3(0.0f, -1.0f, 0.0f);
  normals_[1] = glm::vec3(0.0f, -1.0f, 0.0f);
  normals_[2] = glm::vec3(0.0f, -1.0f, 0.0f);
  normals_[3] = glm::vec3(0.0f, -1.0f, 0.0f);
  // Left
  normals_[4] = glm::vec3(-1.0f, 0.0f, 0.0f);
  normals_[5] = glm::vec3(-1.0f, 0.0f, 0.0f);
  normals_[6] = glm::vec3(-1.0f, 0.0f, 0.0f);
  normals_[7] = glm::vec3(-1.0f, 0.0f, 0.0f);
  // Back
  normals_[8] = glm::vec3(0.0f, 0.0f, -1.0f);
  normals_[9] = glm::vec3(0.0f, 0.0f, -1.0f);
  normals_[10] = glm::vec3(0.0f, 0.0f, -1.0f);
  normals_[11] = glm::vec3(0.0f, 0.0f, -1.0f);
  // Right
  normals_[12] = glm::vec3(1.0f, 0.0f, 0.0f);
  normals_[13] = glm::vec3(1.0f, 0.0f, 0.0f);
  normals_[14] = glm::vec3(1.0f, 0.0f, 0.0f);
  normals_[15] = glm::vec3(1.0f, 0.0f, 0.0f);
  // Front
  normals_[16] = glm::vec3(0.0f, 0.0f, 1.0f);
  normals_[17] = glm::vec3(0.0f, 0.0f, 1.0f);
  normals_[18] = glm::vec3(0.0f, 0.0f, 1.0f);
  normals_[19] = glm::vec3(0.0f, 0.0f, 1.0f);
  // Top
  normals_[20] = glm::vec3(0.0f, 1.0f, 0.0f);
  normals_[21] = glm::vec3(0.0f, 1.0f, 0.0f);
  normals_[22] = glm::vec3(0.0f, 1.0f, 0.0f);
  normals_[23] = glm::vec3(0.0f, 1.0f, 0.0f);

  // Bottom
  elements_[0] = 0;
  elements_[1] = 1;
  elements_[2] = 2;
  
  elements_[3] = 2;
  elements_[4] = 3;
  elements_[5] = 0;
  // Left
  elements_[6] = 4;
  elements_[7] = 5;
  elements_[8] = 6;
  
  elements_[9] = 6;
  elements_[10] = 7;
  elements_[11] = 4;
  // Back
  elements_[12] = 8;
  elements_[13] = 9;
  elements_[14] = 10;
  
  elements_[15] = 10;
  elements_[16] = 11;
  elements_[17] = 8;
  // Right
  elements_[18] = 12;
  elements_[19] = 13;
  elements_[20] = 14;
  
  elements_[21] = 14;
  elements_[22] = 15;
  elements_[23] = 12;
  // Front
  elements_[24] = 16;
  elements_[25] = 17;
  elements_[26] = 18;
  
  elements_[27] = 18;
  elements_[28] = 19;
  elements_[29] = 16;
  // Top
  elements_[30] = 20;
  elements_[31] = 21;
  elements_[32] = 22;
  
  elements_[33] = 22;
  elements_[34] = 23;
  elements_[35] = 20;
  
  transform_matrix_ *= glm::translate(position);

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
void TriangleMesh::initCone(glm::vec3 position,
              glm::vec3 direction,
              glm::vec3 scale,
              int divisions)
{
  // Data sizes
  vertices_.resize(divisions + 2);
  normals_.resize(divisions + 2);
  elements_.resize(divisions * 6);

  // Vertices around the base of the cone
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i=0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    vertices_[i] = glm::vec3(x,y,0);
    normals_[i] = glm::vec3(x,y,0);
  }

  // Top and bottom vertices
  vertices_[divisions] = glm::vec3(0,0,-1);
  normals_[divisions] = glm::vec3(0,0,-1);
  vertices_[divisions + 1] = glm::vec3(0,0,0);
  normals_[divisions + 1] = glm::vec3(0,0,1);
  
  // Set connectivity
  for (int i = 0; i<divisions-1; i++) {
    elements_[i*6] = i;
    elements_[i*6 + 1] = divisions;
    elements_[i*6 + 2] = i + 1;
    elements_[i*6 + 3] = i;
    elements_[i*6 + 4] = i + 1;
    elements_[i*6 + 5] = divisions + 1;
  }
  elements_[divisions*6 - 1 - 5] = divisions - 1;
  elements_[divisions*6 - 1 - 4] = divisions;
  elements_[divisions*6 - 1 - 3] = 0;
  elements_[divisions*6 - 1 - 2] = divisions - 1;
  elements_[divisions*6 - 1 - 1] = 0;
  elements_[divisions*6 - 1 - 0] = divisions + 1;
  
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
  for (int i = 0; i < vertices_.size(); i++) {
    vertices_[i].x *= scale.x;
    vertices_[i].y *= scale.y;
    vertices_[i].z *= scale.z; // Doing only this will make normals wrong
    normals_[i] = glm::vec3(glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0) * M);
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }
  
  transform_matrix_ *= glm::translate(position);
  initialize();
}

//! Initializes a cylinder shape.
/*!
  \param position is the position of the center of the circle
  \param direction is direction of the cone.
  \param scale defines scaling in x-, y-, and z- direction.
  \param divisions is the subdivision level.
*/
void TriangleMesh::initCylinder(glm::vec3 position,
                                glm::vec3 direction,
                                glm::vec3 scale,
                                int divisions)
{
  // Data sizes
  vertices_.resize(divisions * 2 + (divisions + 1) * 2);
  normals_.resize(divisions * 2 + (divisions + 1) * 2);
  elements_.resize(divisions * 12);
  
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i = 0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    
    vertices_[i] = glm::vec3(x,y,1) * 0.5f;
    normals_[i] = glm::vec3(x,y,0);
    
    vertices_[i + divisions] = glm::vec3(x,y,-1) * 0.5f;
    normals_[i + divisions] = glm::vec3(x,y,0);
    
    vertices_[i + 2*divisions] = glm::vec3(x,y,1) * 0.5f;
    normals_[i + 2*divisions] = glm::vec3(0,0,1);
    
    vertices_[i + 3*divisions] = glm::vec3(x,y,-1) * 0.5f;
    normals_[i + 3*divisions] = glm::vec3(0,0,-1);
  }
  vertices_[vertices_.size() - 2] = glm::vec3(0,0,1) * 0.5f;
  normals_[normals_.size() - 2] = glm::vec3(0,0,1);
  vertices_[vertices_.size() - 1] = glm::vec3(0,0,-1) * 0.5f;
  normals_[normals_.size() - 1] = glm::vec3(0,0,-1);
  
  for (int i = 0; i<divisions-1; i++) {
    elements_[i*12] = i;
    elements_[i*12 + 1] = i + divisions;
    elements_[i*12 + 2] = i + 1;
    
    elements_[i*12 + 3] = i + 1;
    elements_[i*12 + 4] = i + divisions;
    elements_[i*12 + 5] = i + 1 + divisions;

    elements_[i*12 + 6] = i + divisions*2;
    elements_[i*12 + 7] = i + divisions*2 + 1;
    elements_[i*12 + 8] = vertices_.size() - 2;
    
    elements_[i*12 + 9] = i + divisions*2 + 1 + divisions;
    elements_[i*12 + 10] = i + divisions*2 + divisions;
    elements_[i*12 + 11] = vertices_.size() - 1;
  }
  
  elements_[divisions*12 - 1 - 11] = divisions - 1;
  elements_[divisions*12 - 1 - 10] = divisions - 1 + divisions;
  elements_[divisions*12 - 1 - 9] = 0;
  
  
  elements_[divisions*12 - 1 - 8] = 0;
  elements_[divisions*12 - 1 - 7] = divisions - 1 + divisions;
  elements_[divisions*12 - 1 - 6] = divisions;
  
  elements_[divisions*12 - 1 - 5] = divisions*2 + divisions - 1;
  elements_[divisions*12 - 1 - 4] = divisions*2 + 0;
  elements_[divisions*12 - 1 - 3] = vertices_.size() - 2;
  
  elements_[divisions*12 - 1 - 2] = divisions*2 + divisions;
  elements_[divisions*12 - 1 - 1] = divisions*2 + divisions - 1 + divisions;
  elements_[divisions*12 - 1 - 0] = vertices_.size() - 1;
  
  glm::mat4 M;
  glm::vec3 up =
  !(direction.x == 0 && direction.y == 0) ?
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  direction,
                  up);
  
  for (int i = 0; i < vertices_.size(); i++) {
    vertices_[i].x *= scale.x;
    vertices_[i].y *= scale.y;
    vertices_[i].z *= scale.z; // Doing only this will make normals wrong
    normals_[i] = glm::vec3(glm::vec4(normals_[i], 0) * M);
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }
  
  transform_matrix_ *= glm::translate(position);

  initialize();
}

void TriangleMesh::initialize()
{
  AbstractMesh::initialize();
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(
    GL_ARRAY_BUFFER,
    normals_.size() * sizeof(glm::vec3),
    &normals_[0],
    GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    elements_.size() * sizeof(unsigned short),
    &elements_[0],
    GL_STATIC_DRAW);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
*/
void TriangleMesh::render(glm::mat4 M, GLuint program_ID)
{
  // Render all children
  Object3D::render(M);
  
  // Use our shader
  glUseProgram(program_ID);

  // Shader input
  glm::mat4 total_transform = M * transform_matrix_;
  glUniformMatrix4fv(
    glGetUniformLocation(program_ID, "M"),
    1,
    GL_FALSE,
    &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
    0,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // 2nd attribute buffer : normals
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glVertexAttribPointer(
    1,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    
  // Draw the triangles
  glDrawElements(
    GL_TRIANGLES,       // mode
    elements_.size(),   // count
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
  glDeleteBuffers(1, &element_buffer_);
}

//! Initializes line from start position to end position.
/*!
  \param start is the start position of the line.
  \param end is the end position of the line.
*/
void LineMesh::initLine(glm::vec3 start, glm::vec3 end)
{
  vertices_.push_back(start);
  vertices_.push_back(end);
  
  elements_.push_back(0);
  elements_.push_back(1);
  
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
  glm::vec3 position,
  glm::vec3 normal,
  glm::vec3 scale,
  unsigned int divisions)
{
  vertices_.resize((divisions + 1) * 4);
  elements_.resize((divisions + 1) * 4);
  int i = 0;
  for (int j=0; i < divisions + 1; i++, j++) {
    vertices_[i] = glm::vec3(j,0.0f,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 2; i++, j++) {
    vertices_[i] = glm::vec3(0.0f,j,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 3; i++, j++) {
    vertices_[i] = glm::vec3(j,divisions,0.0f);
  }
  for (int j=0; i < (divisions + 1) * 4; i++, j++) {
    vertices_[i] = glm::vec3(divisions,j,0.0f);
  }
  
  for (int i = 0; i < elements_.size(); i = i+2) {
    elements_[i] = i/2;
  }
  for (int i = 1; i < elements_.size(); i = i+2) {
    elements_[i] = i/2 + (divisions + 1) * 2;
  }
  
  glm::mat4 M;
  glm::vec3 up =
  !(normal.x == 0 && normal.y == 0) ?
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 0.0f),
    normal,
    up);
  
  for (int i = 0; i < vertices_.size(); i++) {
    vertices_[i] /= divisions;
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }
  
  transform_matrix_ *= glm::translate(position - glm::vec3(0.5f,0.0f,0.5f));
  transform_matrix_ *= glm::scale(scale);

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
                glm::vec3 position,
                glm::vec3 normal,
                glm::vec3 scale,
                unsigned int divisions)
{
  vertices_.resize(divisions);
  elements_.resize(divisions*2);
  
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i = 0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    vertices_[i] = glm::vec3(x,y,0) * 0.5f;
    elements_[i*2] = i;
    elements_[i*2 + 1] = i + 1;
  }
  elements_[divisions*2 - 1] = 0;
  
  glm::mat4 M;
  glm::vec3 up =
    !(normal.x == 0 && normal.y == 0) ?
    glm::vec3(0.0f, 0.0f, 1.0f) :
    glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  normal,
                  up);
  
  for (int i = 0; i < vertices_.size(); i++) {
    //vertices_[i] /= divisions;
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }

  transform_matrix_ *= glm::scale(scale);  
  transform_matrix_ *= glm::translate(position - glm::vec3(0.5f,0.0f,0.5f));
  
  initialize();
}

void LineMesh::initialize()
{
  AbstractMesh::initialize();
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER,
    elements_.size() * sizeof(unsigned short),
    &elements_[0],
    GL_STATIC_DRAW);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
  \param program_ID is the shader that will be bound for rendering.
*/
void LineMesh::render(glm::mat4 M, GLuint program_ID)
{
  Object3D::render(M);
  
  // Use the shader
  glUseProgram(program_ID);
  
  // Input to the shader
  glm::mat4 total_transform = M * transform_matrix_;
  glUniformMatrix4fv(
    glGetUniformLocation(program_ID, "M"),
    1,
    GL_FALSE,
    &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
    0,          // attribute
    3,          // size
    GL_FLOAT,   // type
    GL_FALSE,   // normalized?
    0,          // stride
    (void*)0);  // array buffer offset
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  
  // Draw the triangles
  glDrawElements(
    GL_LINES,           // mode
    elements_.size(),   // count
    GL_UNSIGNED_SHORT,  // type
    (void*)0);          // element array buffer offset


  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

//! Creates a point cloud mesh of size * size particles 
PointCloudMesh::PointCloudMesh(int size) : size_(size)
{
  initialize();
}

void PointCloudMesh::initialize()
{
  AbstractMesh::initialize();
  
  std::vector< std::pair<int, int> >  indices;
  indices.resize(size_ * size_);
  for (int i=0; i<size_; i++) {
    for (int j=0; j<size_; j++) {
      indices[i * size_ + j] = std::pair<int, int>(i,j);
    }
  }
  glGenBuffers(1, &index_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, index_buffer_);
  glBufferData(
    GL_ARRAY_BUFFER,
    indices.size() * sizeof(std::pair<int, int>),
    &indices[0],
    GL_STATIC_DRAW);
}

//! Destructor
PointCloudMesh::~PointCloudMesh()
{
  glDeleteBuffers(1, &index_buffer_);
}

//! Render the mesh.
/*!
  \param M is the transformation matrix of the parent.
  \param program_ID is the shader that will be bound for rendering.
*/
void PointCloudMesh::render(glm::mat4 M, GLuint program_ID)
{
  Object3D::render(M);
    
  // Use our shader
  glUseProgram(program_ID);

  // Input to the shader
  glm::mat4 total_transform = M * transform_matrix_;
  glUniformMatrix4fv(
    glGetUniformLocation(program_ID, "M"),
    1,
    GL_FALSE,
    &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, index_buffer_);
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
