#include <vector>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gl/glew.h>
#include <gl/glfw3.h>

class Object3D {
public:
  Object3D() {};
  virtual ~Object3D() {};
  void addChild(Object3D* child);
  virtual void render();
  // Todo update function here
  // Todo Object3D should inherit transfromation from parent
  
  void translate(float x, float y, float z);
  void scale(float x, float y, float z);
  void rotate(float angle, float x, float y, float z);
  void resetTransform();
protected:
  glm::mat4 transform_;
private:
  std::vector<Object3D*> children;
};

class Mesh : public Object3D{
public:
  Mesh(const char *file_name, GLuint program_ID);
  ~Mesh();
  virtual void render();
private:
  bool initialize(const char *file_name, GLuint program_ID);

  GLuint program_ID_;

  GLuint vertex_array_ID_;
  GLuint vertex_buffer_;
  GLuint normal_buffer_;
  GLuint element_buffer_;
  
  GLuint model_matrix_ID_;
  
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> normals_;
  std::vector<unsigned short> elements_;
};

class Camera : public Object3D {
public:
  Camera(GLuint program_ID);
  void render();
  
  glm::mat4 getViewProjectionMatrix();
private:
  GLuint program_ID_;

  GLuint view_matrix_ID_;
  GLuint projection_matrix_ID_;
  
  glm::mat4 view_transform_;
  glm::mat4 projection_transform_;
};

class SimpleGraphicsEngine {
public:
  SimpleGraphicsEngine();
  ~SimpleGraphicsEngine();
  void run();
protected:
  virtual void update();
  double dt_;
  Object3D* scene_;
  Camera* cam_;
  GLuint program_ID_basic_render;
private:
  bool initialize();

  GLFWwindow* window_;
  
  clock_t start_;
};

class MyGraphicsEngine : public SimpleGraphicsEngine {
public:
  MyGraphicsEngine();
};