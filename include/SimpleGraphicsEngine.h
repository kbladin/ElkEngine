#ifndef SIMPLE_GRAPHICS_ENGINE_H
#define SIMPLE_GRAPHICS_ENGINE_H

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>
#include <gl/glfw3.h>

//! Every TriangleMesh has a material which specifies parameters for shading.
class Material {
public:
  //! Create a material which is bound to a specific shader.
  /*!
   \param program_ID can be one of the shaders which starts with "program_ID_"
   defined in SimpleGraphiicsEngine.
  */
  Material(GLuint program_ID);
  //! Updating the shader parameters.
  /*!
   This function is automatically called when a mesh with this material is
   rendered.
  */
  void render();
  
  glm::vec3 diffuse_color_;
  glm::vec3 specular_color_;
  float specularity_;
  int shinyness_;
private:
  GLuint program_ID_;
  GLuint diffuseColor_ID_;
  GLuint specularColor_ID_;
  GLuint specularity_ID_;
  GLuint shinyness_ID_;
};

//! A transform defining a transformation matrix.
/*!
 Transformation matrix is defined from different parameters such as position,
 scale and rotation.
*/
class Transform {
public:
  Transform();
  
  //! Returns the displacement of the transform.
  /*!
   This is not necessarily the position of the center of the object since that
   position can change with a translation followed by a rotation. This function
   only returns the translation.
  */
  glm::vec3 getPosition(){return position_;};
  //! Returns the scale of the transform.
  glm::vec3 getScale(){return scale_;};
  //! Returns the rotation in terms of EulerXYZ angles.
  glm::vec3 getEulerRotationXYZ(){return rotation_;};
  //! Returns the transformation matrix.
  glm::mat4 getMatrix(){return matrix_;};
  //! Translates the object.
  /*!
   \param position defines translation in each axis.
  */
  void translate(glm::vec3 position);
  //! Scales the object.
  /*!
   \param scale defines scaling in each axis.
  */
  void scale(glm::vec3 scale);
  //! Rotates the object around the x-axis.
  /*!
   \param angle defines the angle of rotation in degrees.
  */
  void rotateX(float angle);
  //! Rotates the object around the y-axis.
  /*!
   \param angle defines the angle of rotation in degrees.
  */
  void rotateY(float angle);
  //! Rotates the object around the z-axis.
  /*!
   \param angle defines the angle of rotation in degrees.
  */
  void rotateZ(float angle);
  //! Resets the matrix transform to an identity matrix.
  void reset();

  glm::mat4 matrix_;

private:
  glm::vec3 position_;
  glm::vec3 scale_;
  glm::vec3 rotation_; // angleX, angleY, angleZ
};

//! An object positioned in 3D space.
/*!
 This object can not be rendered by itself. It needs a child or children in
 form of meshes (TriangleMesh or LineMesh).
 All objects inheriting from Object3D can be added as child.
*/
class Object3D {
public:
  Object3D() {};
  //! Destructor
  /*!
   The children of the Object3D is not destroyed when the Object3D is destroyed.
   The children needs to be destroyed explicitly.
  */
  virtual ~Object3D() {};
  //! Adds a child to the Object3D
  /*!
   \param child is an Object3D pointer that will be added as child.
  */
  void addChild(Object3D* child);
  //! Calling the render function for all of the children
  virtual void render(glm::mat4 M);
  
  Transform transform_;
private:
  std::vector<Object3D*> children;
};

class BoundingBox;

//! This class serves as a base for the mesh classes.
class AbstractMesh : public Object3D{
public:
  AbstractMesh(GLuint shader_ID);
  ~AbstractMesh();
  //! Normalizes the scale of the mesh.
  /*!
   After normalizing, the maximum position of any vertex will be 1.0.
   This function alters the Transform of the AbstractMesh.
  */
  void normalizeScale();
  //! Abstract function for rendering the mesh.
  /*!
   \param M is the transformation matrix of the parent.
  */
  virtual void render(glm::mat4 M) = 0;
  Material material_;
protected:
  virtual void initialize() = 0;
  
  GLuint program_ID_;
  std::vector<glm::vec3> vertices_;
  std::vector<unsigned short> elements_; // Maximum around 60000 vertices for unsigned short.
  GLuint vertex_array_ID_;
  GLuint vertex_buffer_;
  GLuint element_buffer_;
  GLuint model_matrix_ID_;
private:
  friend BoundingBox;
};

//! This class extends AbstractMesh and renders triangles
/*!
 The TriangleMesh also has a list of normals.
*/
class TriangleMesh : public AbstractMesh{
public:
  //! Create a TriangleMesh from a ".m" file.
  /*!
   A shader program also needs to be specified.
   \param file_name is the file path for the model.
   \param program_ID can be one of the shaders which starts with "program_ID_"
   defined in SimpleGraphiicsEngine.
  */
  TriangleMesh(const char *file_name, GLuint program_ID);
  //! Create a TriangleMesh from vertex lists.
  /*!
   \param vertices is a list of vertices.
   \param normals is a list of normals.
   \param elements is a list of elements (indices for faces).
   \param program_ID can be one of the shaders which starts with "program_ID_"
   defined in SimpleGraphiicsEngine.
  */
  TriangleMesh(std::vector<glm::vec3> vertices,
       std::vector<glm::vec3> normals,
       std::vector<unsigned short> elements,
       GLuint program_ID);
  //! Creates a TriangleMesh without initializing vertex lists.
  /*!
   This constructor is used when creating basic TriangleMeshes such as planes
   or boxes.
   First construct the TriangleMesh, then call one of the init-functions
   (eg: initPlane, initBox).
  */
  TriangleMesh(GLuint program_ID);
  ~TriangleMesh();
  //! Initializes a plane.
  /*!
   \param position is a point in the center of the plane.
   \param normal is the normal of the plane.
   \param scale defines scaling in x-, y-, and z- direction.
  */
  void initPlane(glm::vec3 position, glm::vec3 normal, glm::vec3 scale);
  //! Initializes a axis aligned box.
  /*!
   \param max is the max corner of the box
   \param min is the min corner of the box.
   \param position is a point in the center of the box.
  */
  void initBox(glm::vec3 max, glm::vec3 min, glm::vec3 position);
  //! Initializes a cone shape.
  /*!
   Currently, the normals are all soft (not realistic).
   \param position is the position of the center of the circle
   \param direction is direction of the cone.
   \param scale defines scaling in x-, y-, and z- direction.
   \param divisions is the subdivision level.
   */
  void initCone(glm::vec3 position,
                glm::vec3 direction,
                glm::vec3 scale,
                int divisions);
  //! Initializes a cylinder shape.
  /*!
   \param position is the position of the center of the circle
   \param direction is direction of the cone.
   \param scale defines scaling in x-, y-, and z- direction.
   \param divisions is the subdivision level.
   */
  void initCylinder(glm::vec3 position,
                glm::vec3 direction,
                glm::vec3 scale,
                int divisions);
  //! Render the mesh.
  /*!
   \param M is the transformation matrix of the parent.
  */
  virtual void render(glm::mat4 M);
private:
  void initialize();
  GLuint normal_buffer_;
  std::vector<glm::vec3> normals_;
};

//! This class extends AbstractMesh and renders lines
/*!
 Currently, this mesh can not be loaded from file.
*/
class LineMesh : public AbstractMesh {
public:
  //! Creates a TriangleMesh without initializing vertex lists.
  /*!
   This constructor is used when creating basic LineMeshes such as grid-planes
   or axes.
   First construct the LineMesh, then call one of the init-functions
   (eg: initAxes, initGridPlane).
  */
  LineMesh(GLuint program_ID);
  ~LineMesh();
  //! Initializes line from start position to end position.
  /*!
   \param start is the start position of the line.
   \param end is the end position of the line.
  */
  void initLine(glm::vec3 start, glm::vec3 end);
  //! Initializes a grid-plane used as reference in the scene.
  /*!
   \param position is a point in the center of the plane.
   \param normal is the normal of the plane.
   \param scale defines scaling in x-, y-, and z- direction.
   \param divisions defines how dense the grid will be.
  */
  void initGridPlane(
                     glm::vec3 position,
                     glm::vec3 normal,
                     glm::vec3 scale,
                     unsigned int divisions);
  //! Render the mesh.
  /*!
   \param M is the transformation matrix of the parent.
  */
  virtual void render(glm::mat4 M);
private:
  void initialize();
};

//! An axis aligned bounding box.
class BoundingBox{
public:
  //! The BoundingBox can be created from a template mesh.
  /*!
   \param template_mesh is the mesh that will be bound by this BoundingBox.
  */
  BoundingBox(const AbstractMesh* template_mesh);
  //! Returns the max corner of the box.
  glm::vec3 getMin(){return min;}
  //! Returns the min corner of the box.
  glm::vec3 getMax(){return max;}
private:
  glm::vec3 max;
  glm::vec3 min;
};

//! A camera defined in 3D space
class AbstractCamera : public Object3D {
public:
  //! Creates camera to render objects with the defined shader program attached.
  /*!
   \param program_ID is the shader program that this Camera will render.
   \param window is the GLFWwindow* which to render to.
  */
  AbstractCamera(GLuint program_ID, GLFWwindow* window);
  //! Render the Camera.
  /*!
   \param M is the transformation matrix of the parent.
  */
  virtual void render(glm::mat4 M) = 0;
protected:
  GLuint program_ID_;
  GLuint view_matrix_ID_;
  GLuint projection_matrix_ID_;
  
  //glm::mat4 view_transform_;
  glm::mat4 projection_transform_;
  
  GLFWwindow* window_;
};

//! A perspective camera defined in 3D space
class PerspectiveCamera : public AbstractCamera {
public:
  //! Creates camera to render objects with the defined shader program attached.
  /*!
   \param program_ID is the shader program that this Camera will render.
   \param window is the GLFWwindow* which to render to.
   */
  PerspectiveCamera(GLuint program_ID, GLFWwindow* window);
  //! Render the Camera.
  /*!
   \param M is the transformation matrix of the parent.
   */
  virtual void render(glm::mat4 M);
};

//! An orthographic camera defined in 3D space
class OrthoCamera : public AbstractCamera {
public:
  //! Creates camera to render objects with the defined shader program attached.
  /*!
   \param program_ID is the shader program that this Camera will render.
   \param window is the GLFWwindow* which to render to.
   */
  OrthoCamera(GLuint program_ID, GLFWwindow* window);
  //! Render the Camera.
  /*!
   \param M is the transformation matrix of the parent.
   */
  virtual void render(glm::mat4 M);
};

//! A light source defined in 3D space
class LightSource : public Object3D {
public:
  //! Creates a LightSource bound to a specific shader.
  /*!
   \param program_ID is the shader program that this LightSource will be bound
   to.
  */
  LightSource(GLuint program_ID);
  //! Render the LightSource.
  /*!
   \param M is the transformation matrix of the parent.
  */
  virtual void render(glm::mat4 M);
  
  float intensity_;
  glm::vec3 color_;
private:
  GLuint program_ID_;
  
  GLuint light_position_ID_;
  GLuint light_intensity_ID_;
  GLuint light_color_ID_;
};

//! An object to create the x, y and z axes.
class AxesObject3D : public Object3D {
public:
  AxesObject3D(GLuint program_ID, float arrow_size, float axis_radius);
  ~AxesObject3D();

private:
  TriangleMesh* line_x_;
  TriangleMesh* line_y_;
  TriangleMesh* line_z_;
  
  TriangleMesh* arrow_x_;
  TriangleMesh* arrow_y_;
  TriangleMesh* arrow_z_;
};

//! This class manages all objects in the engine.
/*!
 This class has the scene_ which can be used to add more objects by adding
 children to the scene.
 The scene_ has some predefined children such as a grid plane and axes.
 Extend this class to create a program to run.
*/
class SimpleGraphicsEngine {
public:
  //! Initializes OpenGL, creating context and adding all objects for the scene.
  SimpleGraphicsEngine();
  virtual ~SimpleGraphicsEngine();
  
  //! Starts the main loop
  void run();
protected:
  //! This function needs to be implemented if extending this class.
  virtual void update() = 0;
  
  // Probably should be private...
  GLFWwindow* window_;
  
  double dt_;
  
  Object3D* scene_;
  Object3D* view_space_;
  static Object3D* camera_;
  static Object3D* viewspace_ortho_camera_;
  
  GLuint program_ID_basic_render_;
  GLuint program_ID_one_color_shader_;
  
private:
  bool initialize();

  double time_;

  Object3D* grid_plane_;
  Object3D* grid_plane_child1_;
  Object3D* grid_plane_child2_;
  Object3D* grid_plane_child3_;
  LineMesh* grid_plane_mesh_;
  
  AxesObject3D* axis_object_;
  AxesObject3D* axis_object_small_;
  
  // One camera for each shader
  PerspectiveCamera* basic_cam_;
  PerspectiveCamera* one_color_cam_;
  
  OrthoCamera* one_color_ortho_cam_;
};

#endif