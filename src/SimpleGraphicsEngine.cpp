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

void Object3D::render()
{
  for (std::vector<Object3D*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
    (*iter)->render();
  }
}

Mesh::Mesh(const char *file_name, GLuint program_ID)
{
  initialize(file_name, program_ID);
  
  glUseProgram(program_ID_);
  // Get a handle for our matrix uniform
  model_matrix_ID_ = glGetUniformLocation(program_ID_, "M");
}

Mesh::~Mesh()
{
  // Cleanup VBO
  glDeleteBuffers(1, &vertex_buffer_);
  glDeleteBuffers(1, &normal_buffer_);
  glDeleteBuffers(1, &element_buffer_);
  glDeleteVertexArrays(1, &vertex_array_ID_);
  
}

bool Mesh::initialize(const char *file_name, GLuint program_ID)
{
  program_ID_ = program_ID;
  
  if (!ModelLoader::load(
                    file_name,
                    &vertices_,
                    &normals_,
                    &elements_))
    return false;
  
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
  return true;
}

void Mesh::render()
{
  // Use our shader
  glUseProgram(program_ID_);
  glUniformMatrix4fv(model_matrix_ID_, 1, GL_FALSE, &transform_[0][0]);
  
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

Camera::Camera(GLuint program_ID)
{
  program_ID_ = program_ID;
  glUseProgram(program_ID_);
  // Get a handle for our matrix uniform
  view_matrix_ID_ = glGetUniformLocation(program_ID_, "V");
  projection_matrix_ID_ = glGetUniformLocation(program_ID_, "P");
  
  view_transform_ = glm::lookAt(glm::vec3(0,0,1), glm::vec3(0,0,0), glm::vec3(0,1,0));
  projection_transform_ = glm::perspective(45.0f, 3.0f/2.0f, 0.1f, 100.0f);
}

void Camera::render()
{
  glUseProgram(program_ID_);
  glUniformMatrix4fv(view_matrix_ID_, 1, GL_FALSE, &view_transform_[0][0]);
  glUniformMatrix4fv(projection_matrix_ID_, 1, GL_FALSE, &projection_transform_[0][0]);
  glUseProgram(0);
}

SimpleGraphicsEngine::SimpleGraphicsEngine()
{
  initialize();
}

SimpleGraphicsEngine::~SimpleGraphicsEngine()
{
  glDeleteProgram(program_ID_basic_render);
  glfwTerminate();
  delete scene_;
}

bool SimpleGraphicsEngine::initialize()
{
  start_ = std::clock();
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
  
  // Create and compile our GLSL program from the shaders
  program_ID_basic_render = ShaderLoader::loadShaders(
                                               "../../data/shaders/simple.vert",
                                               "../../data/shaders/simple.frag" );
  scene_ = new Object3D;
  cam_ = new Camera(program_ID_basic_render);
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
    glClearColor(0.5, 0.5, 0.5, 1);
    
    cam_->render();
    scene_->render();
    
    /* Swap front and back buffers */
    glfwSwapBuffers(window_);
    
    /* Poll for and process events */
    glfwPollEvents();
  }
}

void SimpleGraphicsEngine::update()
{
  dt_ = ( std::clock() - start_ ) / (double) CLOCKS_PER_SEC;
  start_ = std::clock();
}

MyGraphicsEngine::MyGraphicsEngine() : SimpleGraphicsEngine()
{
  Mesh* bunny = new Mesh("../../data/testmodels/bunny.m", program_ID_basic_render);
  scene_->addChild(bunny);
  

  bunny->scale(3, 3, 3);
  bunny->translate(-0.2, -0.2, 0);
  //hej->rotate(45, 0, 0, 1);
}

#endif