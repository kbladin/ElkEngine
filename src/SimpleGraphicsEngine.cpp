#ifndef SIMPLE_GRAPHICS_ENGINE_H
#define SIMPLE_GRAPHICS_ENGINE_H

#include "SimpleGraphicsEngine.h"

#include <iostream>
#include <vector>

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

void Object3D::addChild(Object3D *child)
{
  children.push_back(child);
}

void Object3D::translate(float x, float y, float z)
{
  transform_ = glm::translate(glm::mat4(), glm::vec3(x,y,z)) *  transform_;
}

void Object3D::scale(float x, float y, float z)
{
  transform_ = glm::scale(glm::mat4(), glm::vec3(x,y,z)) *  transform_;
}

void Object3D::rotate(float angle, float x, float y, float z)
{
  transform_ = glm::rotate(glm::mat4(), angle, glm::vec3(x,y,z)) *  transform_;
}

void Object3D::resetTransform()
{
  transform_ = glm::mat4();
}

void Object3D::render(glm::mat4 M)
{
  for (std::vector<Object3D*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
    (*iter)->render(M * transform_);
  }
}

Mesh::Mesh(const char *file_name, GLuint program_ID) : material_(program_ID)
{
  program_ID_ = program_ID;
  ModelLoader::load(file_name, &vertices_, &normals_, &elements_);
  initialize();
}

Mesh::Mesh(GLuint program_ID) : material_(program_ID)
{
  program_ID_ = program_ID;
}

Mesh::~Mesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
}

void Mesh::initPlane(glm::vec3 position, glm::vec3 normal, glm::vec3 scale)
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
  
  Object3D::scale(scale.x, scale.y, scale.z);
  Object3D::translate(position.x, position.y, position.z);
  
  initialize();
}

void Mesh::initialize()
{
  glUseProgram(program_ID_);

  glGenVertexArrays(1, &vertex_array_ID_);
  glBindVertexArray(vertex_array_ID_);
  
  glGenBuffers(1, &vertex_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), &vertices_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &normal_buffer_);
  glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
  glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), &normals_[0], GL_STATIC_DRAW);
  
  glGenBuffers(1, &element_buffer_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements_.size() * sizeof(unsigned short), &elements_[0] , GL_STATIC_DRAW);
  
    // Get a handle for our matrix uniform
  model_matrix_ID_ = glGetUniformLocation(program_ID_, "M");
}

void Mesh::render(glm::mat4 M)
{
  Object3D::render(M);
  material_.render();
  
  glm::mat4 total_transform = M * transform_;
  
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

LineMesh::LineMesh(GLuint program_ID) : material_(program_ID)
{
  program_ID_ = program_ID;

  vertices_.push_back(glm::vec3(0.0f,0.0f,0.0f));
  vertices_.push_back(glm::vec3(1.0f,0.0f,0.0f));
  vertices_.push_back(glm::vec3(0.0f,1.0f,0.0f));
  vertices_.push_back(glm::vec3(0.0f,0.0f,1.0f));
  
  colors_.push_back(glm::vec3(1.0f,1.0f,1.0f));
  colors_.push_back(glm::vec3(1.0f,0.0f,0.0f));
  colors_.push_back(glm::vec3(0.0f,1.0f,0.0f));
  colors_.push_back(glm::vec3(0.0f,0.0f,1.0f));
  
  elements_.push_back(0);
  elements_.push_back(1);
  elements_.push_back(0);
  elements_.push_back(2);
  elements_.push_back(0);
  elements_.push_back(3);
  
  initialize();
}

LineMesh::~LineMesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &color_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
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
  Object3D::render(M);
  material_.render();
  
  glm::mat4 total_transform = M * transform_;
  
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

Camera::Camera(GLuint program_ID, GLFWwindow* window)
{
  window_ = window;
  program_ID_ = program_ID;
  glUseProgram(program_ID_);
  // Get a handle for our matrix uniform
  view_matrix_ID_ = glGetUniformLocation(program_ID_, "V");
  projection_matrix_ID_ = glGetUniformLocation(program_ID_, "P");
  
  view_transform_ = glm::lookAt(
                                glm::vec3(0.5f,0.5f,1.0f) * 1.0f,
                                glm::vec3(0.0f,0.0f,0.0f),
                                glm::vec3(0.0f,1.0f,0.0f));
  
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
}

void Camera::render(glm::mat4 M)
{
  Object3D::render(M * transform_);
  
  int width;
  int height;
  glfwGetWindowSize(window_, &width, &height);
  float aspect = float(width)/height;
  projection_transform_ = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

  glm::mat4 V = view_transform_ * M * transform_;
  
  glUseProgram(program_ID_);

  glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &V[0][0]);
  glUniformMatrix4fv(projection_matrix_ID_, 1, GL_FALSE, &projection_transform_[0][0]);
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
  Object3D::render(M * transform_);
  
  glm::vec4 position = M * transform_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  //glm::vec4 direction = M * transform_ * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
  
  glUseProgram(program_ID_);
  glUniform3f(light_position_ID_,position.x,position.y, position.z);
  glUniform1f(light_intensity_ID_, intensity_);
  glUniform3f(light_color_ID_, color_.r, color_.g, color_.b);
}

SimpleGraphicsEngine::SimpleGraphicsEngine()
{
  initialize();
}

SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  glDeleteProgram(program_ID_basic_render_);
  glfwTerminate();
  delete scene_;
  //delete cam_;
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
  window_ = glfwCreateWindow(640, 480, "Model Viewer", NULL, NULL);
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
  program_ID_axis_shader_ = ShaderLoader::loadShaders(
                                                      "../../data/shaders/axis.vert",
                                                      "../../data/shaders/axis.frag" );
  // Create and compile our GLSL program from the shaders
  program_ID_grid_plane_shader_ = ShaderLoader::loadShaders(
                                                     "../../data/shaders/grid_plane.vert",
                                                     "../../data/shaders/grid_plane.frag" );
  scene_ = new Object3D;
  master_cam_ = new Object3D();// Camera(program_ID_basic_render_, window_);
  basic_cam_ = new Camera(program_ID_basic_render_, window_);
  axis_cam_ = new Camera(program_ID_axis_shader_, window_);
  grid_plane_cam_ = new Camera(program_ID_grid_plane_shader_, window_);
  
  master_cam_->addChild(basic_cam_);
  master_cam_->addChild(axis_cam_);
  master_cam_->addChild(grid_plane_cam_);
  
  axes_ = new LineMesh(program_ID_axis_shader_);
  grid_plane_ = new Mesh(program_ID_grid_plane_shader_);
  grid_plane_->initPlane(
                         glm::vec3(0.0f,0.0f,0.0f),
                         glm::vec3(0.0f,1.0f,0.0f),
                         glm::vec3(0.5f,0.5f,0.5f));
  
  scene_->addChild(master_cam_);
  scene_->addChild(axes_);
  scene_->addChild(grid_plane_);
  return true;
}

void SimpleGraphicsEngine::run()
{
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window_))
  {
    update();
    
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.7, 0.7, 0.7, 1);
    
    scene_->render(glm::mat4());
    
    /* Swap front and back buffers */
    glfwSwapBuffers(window_);
    
    /* Poll for and process events */
    glfwPollEvents();
  }
}

void SimpleGraphicsEngine::update()
{
  dt_ = glfwGetTime() - time_;
  time_ = glfwGetTime();
}

MyGraphicsEngine::MyGraphicsEngine() : SimpleGraphicsEngine()
{

  light_ = new LightSource(program_ID_basic_render_);
  light_->translate(1, 3, 0);
  bunny_mesh_ = new Mesh("../../data/testmodels/bunny.m", program_ID_basic_render_);
  bunny_ = new Object3D();
  bunny_->addChild(bunny_mesh_);
  //bunny_->addChild(bunny_child);
  bunny_->scale(1, 1, 1);
  bunny_->rotate(180, 0, 1, 0);
  
  scene_->addChild(bunny_);
  scene_->addChild(light_);
}

MyGraphicsEngine::~MyGraphicsEngine()
{
  delete bunny_;
  delete bunny_mesh_;
  delete light_;
}

void MyGraphicsEngine::update()
{
  SimpleGraphicsEngine::update();
  //bunny_->rotate(10 * dt_, 0, 1, 0);
  master_cam_->rotate(10 * dt_, 0, 1, 0);
}

#endif