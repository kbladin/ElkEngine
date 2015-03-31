#include "SimpleGraphicsEngine.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>
#include <gl/glfw3.h>

#include "ModelLoader.h"
#include "ShaderLoader.h"

Material::Material(GLuint program_ID)
{
  diffuse_color_ = glm::vec3(1.0, 1.0, 1.0);
  specular_color_ = glm::vec3(1.0, 1.0, 1.0);
  specularity_ = 0.5;
  shinyness_ = 16;
  
  program_ID_ = program_ID;
  
  glUseProgram(program_ID_);
  diffuseColor_ID_ = glGetUniformLocation(program_ID_, "material_diffiseColor");
  specularColor_ID_ = glGetUniformLocation(program_ID_, "material_specularColor");
  specularity_ID_ = glGetUniformLocation(program_ID_, "material_specularity");
  shinyness_ID_ = glGetUniformLocation(program_ID_, "material_shinyness");
}

void Material::render()
{
  // Use our shader
  glUseProgram(program_ID_);
  glUniform3f(diffuseColor_ID_,diffuse_color_.r,diffuse_color_.g, diffuse_color_.b);
  glUniform3f(specularColor_ID_,specular_color_.r,specular_color_.g, specular_color_.b);
  glUniform1f(specularity_ID_, specularity_);
  glUniform1i(shinyness_ID_, shinyness_);
}

Transform::Transform(){
  reset();
}

void Transform::translate(glm::vec3 position)
{
  glm::mat4 translation_matrix = glm::translate(glm::mat4(), position);
  position_ = glm::vec3( translation_matrix * glm::vec4(position_, 1));
  matrix_ =  translation_matrix * matrix_;
}

void Transform::scale(glm::vec3 scale)
{
  glm::mat4 scale_matrix = glm::scale(glm::mat4(), scale);
  scale_ = glm::vec3( scale_matrix * glm::vec4(scale_, 1));
  matrix_ = scale_matrix * matrix_;
}

void Transform::rotateX(float angle)
{
  rotation_.x += angle;
  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(), angle, glm::vec3(1,0,0));
  matrix_ = rotation_matrix * matrix_;
}

void Transform::rotateY(float angle)
{
  rotation_.y += angle;
  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(), angle, glm::vec3(0,1,0));
  matrix_ = rotation_matrix * matrix_;
}

void Transform::rotateZ(float angle)
{
  rotation_.z += angle;
  glm::mat4 rotation_matrix = glm::rotate(glm::mat4(), angle, glm::vec3(0,0,1));
  matrix_ = rotation_matrix * matrix_;
}

void Transform::setBasis(glm::mat4 basis)
{
  basis_ = basis;
}

void Transform::reset()
{
  position_ = glm::vec3(0.0f,0.0f,0.0f);
  scale_ = glm::vec3(1.0f,1.0f,1.0f);
  matrix_ = glm::mat4();
}

void Object3D::addChild(Object3D *child)
{
  children.push_back(child);
}

void Object3D::removeChild(Object3D *child)
{
  children.erase(std::remove(children.begin(), children.end(), child), children.end());
  for (int i=0; i<children.size(); i++) {
    children[i]->removeChild(child);
  }
}

void Object3D::render(glm::mat4 M)
{
  for (std::vector<Object3D*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
    (*iter)->render(M * transform_.getMatrix());
  }
}

std::vector<Object3D*> Object3D::getCollidingChildren(glm::vec3 point)
{
  std::vector<Object3D*> result;
  for (int i=0; i<children.size(); i++) {
    if(dynamic_cast<BoundingBox*>(children[i]) && dynamic_cast<BoundingBox*>(children[i])->collides(glm::vec3(glm::inverse(transform_.getMatrix()) * glm::vec4(point,1)))) {
      //std::cout << glm::vec3(glm::inverse(transform_.matrix_) * glm::vec4(point,1)).x << std::endl;
      result.push_back(this);
    }
    std::vector<Object3D*> children_result = children[i]->getCollidingChildren(glm::vec3(transform_.getMatrix() * glm::vec4(point,1)));
    for (int j=0; j<children_result.size(); j++) {
      result.push_back(children_result[j]);
    }
  }
  return result;
}

AbstractMesh::AbstractMesh(GLuint program_ID) : material_(program_ID)
{
  program_ID_ = program_ID;
}

AbstractMesh::~AbstractMesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
}

void AbstractMesh::normalizeScale()
{
  BoundingBox bb(this);
  float scale_max = glm::max(bb.getMax().x, glm::max(bb.getMax().y, bb.getMax().z));
  float scale_min = glm::max(-bb.getMin().x, glm::max(-bb.getMin().y, -bb.getMin().z));
  float scale = glm::max(scale_max, scale_min);
  
  transform_.scale(glm::vec3(1/scale, 1/scale, 1/scale));
}

void AbstractMesh::initialize()
{
  glUseProgram(program_ID_);
  
  glGenVertexArrays(1, &vertex_array_ID_);
  glBindVertexArray(vertex_array_ID_);
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(unsigned short), &elements_[0] , GL_STATIC_DRAW);
  
  // Get a handle for our matrix uniform
  model_matrix_ID_ = glGetUniformLocation(program_ID_, "M");
}

TriangleMesh::TriangleMesh(const char *file_name, GLuint program_ID) : AbstractMesh(program_ID)
{
  program_ID_ = program_ID;
  ModelLoader::load(file_name, &vertices_, &normals_, &elements_);
  initialize();
}

TriangleMesh::TriangleMesh(std::vector<glm::vec3> vertices,
           std::vector<glm::vec3> normals,
           std::vector<unsigned short> elements,
           GLuint program_ID) : AbstractMesh(program_ID)
{
  program_ID_ = program_ID;
  vertices_ = vertices;
  normals_ = normals;
  elements_ = elements;
  
  initialize();
}

TriangleMesh::TriangleMesh(GLuint program_ID) : AbstractMesh(program_ID)
{
  program_ID_ = program_ID;
}

TriangleMesh::~TriangleMesh()
{
  glDeleteBuffers(1, &normal_buffer_);
}

void TriangleMesh::initPlane(glm::vec3 position, glm::vec3 normal, glm::vec3 scale)
{
  vertices_.resize(4);
  normals_.resize(4);
  elements_.resize(6);
  
  vertices_[0] = glm::vec3(0.5f, 0.5f, 0.0f);
  vertices_[1] = glm::vec3(0.5f, -0.5f, 0.0f);
  vertices_[2] = glm::vec3(-0.5f, -0.5f, 0.0f);
  vertices_[3] = glm::vec3(-0.5f, 0.5f, 0.0f);
  
  elements_[0] = 0;
  elements_[1] = 1;
  elements_[2] = 2;
  elements_[3] = 2;
  elements_[4] = 3;
  elements_[5] = 0;
  
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
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }

  transform_.scale(scale);
  transform_.translate(position);
  
  initialize();
}

void TriangleMesh::initBox(glm::vec3 max,
                   glm::vec3 min,
                   glm::vec3 position)
{
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
  
  transform_.translate(position);
  
  initialize();
}

void TriangleMesh::initCone(glm::vec3 position,
              glm::vec3 direction,
              glm::vec3 scale,
              int divisions)
{
  vertices_.resize(divisions + 2);
  normals_.resize(divisions + 2);
  elements_.resize(divisions * 6);
  float delta_theta = M_PI * 2 / float(divisions);
  for (int i=0; i<divisions; i++) {
    float x = cos(delta_theta * i);
    float y = sin(delta_theta * i);
    vertices_[i] = glm::vec3(x,y,0);
    normals_[i] = glm::vec3(x,y,0);
  }
  vertices_[divisions] = glm::vec3(0,0,-1);
  normals_[divisions] = glm::vec3(0,0,-1);
  vertices_[divisions + 1] = glm::vec3(0,0,0);
  normals_[divisions + 1] = glm::vec3(0,0,1);
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
    normals_[i] = glm::vec3(glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0) * M);
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }
  
  transform_.translate(position);
  initialize();
}

void TriangleMesh::initCylinder(glm::vec3 position,
                                glm::vec3 direction,
                                glm::vec3 scale,
                                int divisions)
{
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
  
  transform_.translate(position);

  initialize();
}

void TriangleMesh::initialize()
{
  AbstractMesh::initialize();
  
  glUseProgram(program_ID_);

  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);
}

void TriangleMesh::render(glm::mat4 M)
{
  Object3D::render(M);
  material_.render();
  
  glm::mat4 total_transform = M * transform_.getMatrix();
  
  // Use our shader
  glUseProgram(program_ID_);
  glUniformMatrix4fv(model_matrix_ID_, 1, GL_FALSE, &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glVertexAttribPointer(
                        1,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  
  // Draw the triangles !
  glDrawElements(
                 GL_TRIANGLES,      // mode
                 elements_.size(),    // count
                 GL_UNSIGNED_SHORT,   // type
                 (void*)0           // element array buffer offset
                 );
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

LineMesh::LineMesh(GLuint program_ID) : AbstractMesh(program_ID)
{
  program_ID_ = program_ID;
  initialize();
}

LineMesh::~LineMesh()
{

}

void LineMesh::initLine(glm::vec3 start, glm::vec3 end)
{
  vertices_.push_back(start);
  vertices_.push_back(end);
  
  elements_.push_back(0);
  elements_.push_back(1);
  
  initialize();
}

void LineMesh::initGridPlane(glm::vec3 position,
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
  
  transform_.translate(position - glm::vec3(0.5f,0.0f,0.5f));
  transform_.scale(scale);
  
  initialize();
}

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
  glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
  M = glm::lookAt(
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  normal,
                  up);
  
  for (int i = 0; i < vertices_.size(); i++) {
    //vertices_[i] /= divisions;
    vertices_[i] = glm::vec3(glm::vec4(vertices_[i], 1) * M);
  }
  
  transform_.scale(scale);
  transform_.translate(position);
  
  initialize();
}

void LineMesh::initialize()
{
  glUseProgram(program_ID_);

  glGenVertexArrays(1, &vertex_array_ID_);
  glBindVertexArray(vertex_array_ID_);
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(unsigned short), &elements_[0] , GL_STATIC_DRAW);
  
  // Get a handle for our matrix uniform
  model_matrix_ID_ = glGetUniformLocation(program_ID_, "M");
}

void LineMesh::render(glm::mat4 M)
{
  material_.render();
  Object3D::render(M);
  
  glm::mat4 total_transform = M * transform_.getMatrix();
  
  // Use our shader
  glUseProgram(program_ID_);
  glUniformMatrix4fv(model_matrix_ID_, 1, GL_FALSE, &total_transform[0][0]);
  
  glBindVertexArray(vertex_array_ID_);
  
  // 1rst attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glVertexAttribPointer(
                        0,                  // attribute
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );
  
  // Index buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  
  // Draw the triangles !
  glDrawElements(
                 GL_LINES,      // mode
                 elements_.size(),    // count
                 GL_UNSIGNED_SHORT,   // type
                 (void*)0           // element array buffer offset
                 );
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

BoundingBox::BoundingBox(const AbstractMesh* template_mesh)
{
  float max_x = template_mesh->vertices_[0].x;
  float max_y = template_mesh->vertices_[0].y;
  float max_z = template_mesh->vertices_[0].z;
  
  float min_x = template_mesh->vertices_[0].x;
  float min_y = template_mesh->vertices_[0].y;
  float min_z = template_mesh->vertices_[0].z;
  for (int i = 1; i < template_mesh->vertices_.size(); i++) {
    max_x = glm::max(max_x, template_mesh->vertices_[i].x);
    max_y = glm::max(max_y, template_mesh->vertices_[i].y);
    max_z = glm::max(max_z, template_mesh->vertices_[i].z);

    min_x = glm::min(min_x, template_mesh->vertices_[i].x);
    min_y = glm::min(min_y, template_mesh->vertices_[i].y);
    min_z = glm::min(min_z, template_mesh->vertices_[i].z);
  }
  max = glm::vec3(max_x, max_y, max_z);
  min = glm::vec3(min_x, min_y, min_z);
}

BoundingBox::BoundingBox(const Object3D)
{
  
}

bool BoundingBox::collides(glm::vec3 point)
{
  return (point.x > min.x &&
          point.y > min.y &&
          point.z > min.z &&
          point.x < max.x &&
          point.y < max.y &&
          point.z < max.z);
}

AbstractCamera::AbstractCamera(GLuint program_ID, GLFWwindow* window)
{
  window_ = window;
  program_ID_ = program_ID;
  glUseProgram(program_ID_);
  // Get a handle for our matrix uniform
  view_matrix_ID_ = glGetUniformLocation(program_ID_, "V");
  projection_matrix_ID_ = glGetUniformLocation(program_ID_, "P");
  
  /*view_transform_ = glm::mat4(); glm::lookAt(
                                glm::vec3(0.0f,0.0f,0.0f),
                                glm::vec3(0.0f,0.0f,-1.0f),
                                glm::vec3(0.0f,1.0f,0.0f));*/
}

void AbstractCamera::render(glm::mat4 M)
{
  Object3D::render(M * transform_.getMatrix());

  glm::mat4 V = M * transform_.getMatrix();
  
  glUseProgram(program_ID_);

  glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(projection_matrix_ID_, 1, GL_FALSE, &projection_transform_[0][0]);
}

PerspectiveCamera::PerspectiveCamera(GLuint program_ID, GLFWwindow* window) :
AbstractCamera(program_ID, window)
{
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
}

void PerspectiveCamera::render(glm::mat4 M)
{
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
  
  AbstractCamera::render(M);
}

OrthoCamera::OrthoCamera(GLuint program_ID, GLFWwindow* window) :
  AbstractCamera(program_ID, window)
{
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -100.0f, 100.0f);
}

void OrthoCamera::render(glm::mat4 M)
{
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -100.0f, 100.0f);
  
  AbstractCamera::render(M);
}

LightSource::LightSource(GLuint program_ID)
{
  intensity_ = 5.0f;
  color_ = glm::vec3(1.0, 1.0, 1.0);
  
  program_ID_ = program_ID;
  
  glUseProgram(program_ID_);
  light_position_ID_ = glGetUniformLocation(program_ID_, "lightPosition");
  light_intensity_ID_ = glGetUniformLocation(program_ID_, "lightIntensity");
  light_color_ID_ = glGetUniformLocation(program_ID_, "lightColor");
}

void LightSource::render(glm::mat4 M)
{
  Object3D::render(M * transform_.getMatrix());
  
  glm::vec4 position = M * transform_.getMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  
  glUseProgram(program_ID_);
  glUniform3f(light_position_ID_,position.x,position.y, position.z);
  glUniform1f(light_intensity_ID_, intensity_);
  glUniform3f(light_color_ID_, color_.r, color_.g, color_.b);
}

AxesObject3D::AxesObject3D(GLuint program_ID,
                           float arrow_size,
                           float axis_radius)
{
  line_x_ = new TriangleMesh(program_ID);
  line_y_ = new TriangleMesh(program_ID);
  line_z_ = new TriangleMesh(program_ID);
  
  arrow_x_ = new TriangleMesh(program_ID);
  arrow_y_ = new TriangleMesh(program_ID);
  arrow_z_ = new TriangleMesh(program_ID);
  
  glm::vec3 arrow_size_vec = glm::vec3(0.3,0.3,1) * arrow_size;
  int divisions = 10;
  
  line_x_->initCylinder(glm::vec3(0.5 - arrow_size_vec.z/2,0,0),
                        glm::vec3(1,0,0),
                        glm::vec3(axis_radius * 2,
                                  axis_radius * 2,
                                  1 - arrow_size_vec.z),
                        divisions);
  line_y_->initCylinder(glm::vec3(0,0.5 - arrow_size_vec.z/2,0),
                        glm::vec3(0,1,0),
                        glm::vec3(axis_radius * 2,
                                  axis_radius * 2,
                                  1 - arrow_size_vec.z),
                        divisions);
  line_z_->initCylinder(glm::vec3(0,0,0.5 - arrow_size_vec.z/2),
                        glm::vec3(0,0,1),
                        glm::vec3(axis_radius * 2,
                                  axis_radius * 2,
                                  1 - arrow_size_vec.z),
                        divisions);
  line_x_->material_.diffuse_color_ = glm::vec3(1,0,0);
  line_y_->material_.diffuse_color_ = glm::vec3(0,1,0);
  line_z_->material_.diffuse_color_ = glm::vec3(0,0,1);


  
  arrow_x_->initCone(glm::vec3(1 - arrow_size_vec.z,0,0),
                 glm::vec3(1,0,0),
                 arrow_size_vec,
                 divisions);

  arrow_y_->initCone(glm::vec3(0,1 - arrow_size_vec.z,0),
                    glm::vec3(0,1,0),
                    arrow_size_vec,
                    divisions);
  
  arrow_z_->initCone(glm::vec3(0,0,1 - arrow_size_vec.z),
                    glm::vec3(0,0,1),
                    arrow_size_vec,
                    divisions);
  
  arrow_x_->material_.diffuse_color_ = glm::vec3(1,0,0);
  arrow_y_->material_.diffuse_color_ = glm::vec3(0,1,0);
  arrow_z_->material_.diffuse_color_ = glm::vec3(0,0,1);

  this->addChild(line_x_);
  this->addChild(line_y_);
  this->addChild(line_z_);
  
  this->addChild(arrow_x_);
  this->addChild(arrow_y_);
  this->addChild(arrow_z_);
}

AxesObject3D::~AxesObject3D()
{
  delete line_x_;
  delete line_y_;
  delete line_z_;
  
  delete arrow_x_;
  delete arrow_y_;
  delete arrow_z_;
}

LightMesh3D::LightMesh3D(GLuint program_ID, float size)
{
  circle_x_ = new LineMesh(program_ID);
  circle_y_ = new LineMesh(program_ID);
  circle_z_ = new LineMesh(program_ID);
  circle_x_->initCircle(glm::vec3(0,0,0), glm::vec3(1,0,0), size * glm::vec3(1,1,1), 10);
  circle_y_->initCircle(glm::vec3(0,0,0), glm::vec3(0,1,0), size * glm::vec3(1,1,1), 10);
  circle_z_->initCircle(glm::vec3(0,0,0), glm::vec3(0,0,1), size * glm::vec3(1,1,1), 10);
  this->addChild(circle_x_);
  this->addChild(circle_y_);
  this->addChild(circle_z_);
}

LightMesh3D::~LightMesh3D()
{
  delete circle_x_;
  delete circle_y_;
  delete circle_z_;
}

Object3D* SimpleGraphicsEngine::camera_;
Object3D* SimpleGraphicsEngine::viewspace_ortho_camera_;

SimpleGraphicsEngine::SimpleGraphicsEngine()
{
  initialize();
}

SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  glDeleteProgram(program_ID_basic_render_);
  glDeleteProgram(program_ID_one_color_shader_);
  glDeleteProgram(program_ID_background_shader_);
  glfwTerminate();
  delete scene_;
  delete view_space_;
  delete background_space_;
  delete background_plane_;
  delete grid_plane_;
  delete camera_;
  delete basic_cam_;
  delete one_color_cam_;
  delete one_color_ortho_cam_;
}

bool SimpleGraphicsEngine::initialize()
{
  time_ = glfwGetTime();
  // Initialize the library
  if (!glfwInit())
    return -1;
  // Modern OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create a windowed mode window and its OpenGL context
  window_ = glfwCreateWindow(720, 480, "Model Viewer", NULL, NULL);
  if (!window_)
  {
    glfwTerminate();
    return false;
  }
  // Make the window's context current
  glfwMakeContextCurrent(window_);
  printf("%s\n", glGetString(GL_VERSION));
  
  glewExperimental=true; // Needed in core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return false;
  }
  
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);
  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);
  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  // Create and compile our GLSL program from the shaders
  program_ID_basic_render_ = ShaderLoader::loadShaders(
                                               "../../data/shaders/simple.vert",
                                               "../../data/shaders/simple.frag" );
  // Create and compile our GLSL program from the shaders
  program_ID_one_color_shader_ = ShaderLoader::loadShaders(
                                                      "../../data/shaders/one_color.vert",
                                                      "../../data/shaders/one_color.frag" );
  // Create and compile our GLSL program from the shaders
  program_ID_background_shader_ = ShaderLoader::loadShaders(
                                                           "../../data/shaders/background.vert",
                                                           "../../data/shaders/background.frag" );
  scene_ = new Object3D();
  view_space_ = new Object3D();
  background_space_ = new Object3D();
  camera_ = new Object3D();
  viewspace_ortho_camera_ = new Object3D();
  basic_cam_ = new PerspectiveCamera(program_ID_basic_render_, window_);
  one_color_cam_ = new PerspectiveCamera(program_ID_one_color_shader_, window_);
  one_color_cam_ = new PerspectiveCamera(program_ID_one_color_shader_, window_);
  one_color_ortho_cam_ = new OrthoCamera(program_ID_one_color_shader_, window_);
  background_ortho_cam_ = new OrthoCamera(program_ID_background_shader_, window_);
  
  camera_->addChild(basic_cam_);
  camera_->addChild(one_color_cam_);
  viewspace_ortho_camera_->addChild(one_color_ortho_cam_);
  viewspace_ortho_camera_->addChild(background_ortho_cam_);
  
  grid_plane_ = new Object3D();
  grid_plane_child1_ = new Object3D();
  grid_plane_child2_ = new Object3D();
  grid_plane_child3_ = new Object3D();
  grid_plane_mesh_ = new LineMesh(program_ID_one_color_shader_);
  grid_plane_mesh_->initGridPlane(glm::vec3(0.0f,0.0f,0.0f),
                             glm::vec3(0.0f,1.0f,0.0f),
                             glm::vec3(2.0f,2.0f,2.0f),
                             10);
  grid_plane_mesh_->material_.diffuse_color_ = glm::vec3(0.8,0.8,0.8);
  axis_object_ = new AxesObject3D(program_ID_one_color_shader_, 0.1, 0.005);
  axis_object_small_ = new AxesObject3D(program_ID_one_color_shader_, 0.2, 0.02);
  axis_object_small_->transform_.scale(glm::vec3(0.2,0.2,0.2));
  
  background_plane_ = new TriangleMesh(program_ID_background_shader_);
  background_plane_->initPlane(glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(10,2,2));
  
  grid_plane_child1_->addChild(grid_plane_mesh_);
  grid_plane_child2_->addChild(grid_plane_mesh_);
  grid_plane_child3_->addChild(grid_plane_mesh_);
  grid_plane_->addChild(grid_plane_child1_);
  grid_plane_child1_->addChild(grid_plane_child2_);
  grid_plane_child2_->addChild(grid_plane_child3_);
  
  grid_plane_child2_->transform_.scale(glm::vec3(5,5,5));
  grid_plane_child3_->transform_.scale(glm::vec3(5,5,5));
  
  scene_->addChild(camera_);
  scene_->addChild(grid_plane_);
  scene_->addChild(axis_object_);
  
  view_space_->addChild(viewspace_ortho_camera_);
  view_space_->addChild(axis_object_small_);
  
  background_space_->addChild(background_plane_);
  return true;
}

void SimpleGraphicsEngine::run()
{
  while (!glfwWindowShouldClose(window_))
  {
    update();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.7, 0.7, 0.7, 1);
    
    glDisable(GL_DEPTH_TEST);
    background_space_->render(glm::mat4());
    glEnable(GL_DEPTH_TEST);
    scene_->render(glm::mat4());
    view_space_->render(glm::mat4());

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

void SimpleGraphicsEngine::update()
{
  dt_ = glfwGetTime() - time_;
  time_ = glfwGetTime();
  
  glm::vec3 rot = camera_->transform_.getEulerRotationXYZ();
  Transform t;
  
  t.rotateY(rot.y);
  t.rotateX(rot.x);
  t.rotateZ(rot.z);
  t.scale(glm::vec3(0.1,0.1,0.1));
  t.translate(glm::vec3(-1.3,-0.8,0));

  axis_object_small_->transform_ = t;
}