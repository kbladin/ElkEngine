#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/core/simple_graphics_engine.h>
#include <SGE/window/application_window_glfw.h>
#include <SGE/core/shader_manager.h>
#include <SGE/core/new_mesh.h>
#include "sge/core/create_mesh.h"
#include "sge/core/create_texture.h"
#include "sge/core/texture_unit.h"
#include "sge/core/frame_buffer_object.h"
#include "sge/core/render_buffer_object.h"
#include "sge/core/renderer.h"

#include <functional>
#include <memory>

using namespace sge::core;
using namespace sge::window;

class Grid : public Object3D
{
public:
  Grid(ShaderProgram* program);
  ~Grid(){};
  virtual void execute() override;
private:
  ShaderProgram* _program;
  std::shared_ptr<NewMesh> _mesh;
};

Grid::Grid(ShaderProgram* program) :
_program(program)
{
  _mesh = CreateMesh::grid(20);
}

void Grid::execute()
{
  _program->pushUsage();
  
    glUniformMatrix4fv(
      glGetUniformLocation(_program->id(), "M"),
      1,
      GL_FALSE,
      &absoluteTransform()[0][0]);

  _mesh->render();
  
  _program->popUsage();
}

class RenderableBox : public Object3D
{
public:
    RenderableBox();
    ~RenderableBox(){};
    virtual void execute() override;
private:
    std::shared_ptr<NewMesh> _new_mesh;
    std::shared_ptr<Texture> _tex;
};

RenderableBox::RenderableBox()
{
  _new_mesh = CreateMesh::load("../../data/meshes/suzanne.obj");
  _tex = CreateTexture::white(100, 100);
  _tex->uploadTexture();
}

void RenderableBox::execute()
{
  TextureUnit tex_unit;
  tex_unit.activate();
  _tex->bind();

  glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(), "tex"), 0);

  glUniformMatrix4fv(
    glGetUniformLocation(ShaderProgram::currentProgramId(), "M"),
    1,
    GL_FALSE,
    &absoluteTransform()[0][0]);


  _new_mesh->render();
}

class FrameBufferQuad : public Object3D
{
public:
  FrameBufferQuad(int width, int height);
  ~FrameBufferQuad();
  virtual void execute() override;
  inline void bindFBO() { _fbo.bind(); };
  inline void unbindFBO() { _fbo.unbind(); };
  inline int width() { return _width; };
  inline int height() { return _height; };
private:
  int _width, _height;
  std::shared_ptr<NewMesh> _quad;
  FrameBufferObject _fbo;
  std::shared_ptr<Texture> _render_texture;
  RenderBufferObject _depth_buffer;
};

FrameBufferQuad::FrameBufferQuad(int width, int height) :
  _width(width),
  _height(height),
  _depth_buffer(width, height)
{
  _quad = CreateMesh::quad();
  _render_texture = CreateTexture::white(_width, _height);
  _render_texture->uploadTexture();
  _fbo.attach2DTexture(_render_texture->id(), GL_COLOR_ATTACHMENT0, 0);
  _fbo.attachRenderBuffer(_depth_buffer.id(), GL_DEPTH_ATTACHMENT);
}

FrameBufferQuad::~FrameBufferQuad()
{

}

void FrameBufferQuad::execute()
{
  TextureUnit tex_unit;
  tex_unit.activate();
  _render_texture->bind();

  glUniform1i(glGetUniformLocation(ShaderProgram::currentProgramId(), "tex"), 0);

  _quad->render();
}

class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update();
private:
  std::unique_ptr<Renderer> _phong_renderer;
  std::unique_ptr<Renderer> _fbo_renderer;

  std::unique_ptr<RenderableBox> _box;
  std::unique_ptr<Grid> _grid0;
  std::unique_ptr<Grid> _grid1;
  std::unique_ptr<Grid> _grid2;
  
  std::unique_ptr<FrameBufferQuad> _fbo_quad;
};

MyEngine::MyEngine() : SimpleGraphicsEngine(720, 480)
{
  ShaderManager::instance().loadAndAddShader(
    "simple_shader_program",
    "../../shaders/simple.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/simple.frag");

  ShaderManager::instance().loadAndAddShader(
    "simple_white",
    "../../shaders/simple_white.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/simple_white.frag");

  ShaderManager::instance().loadAndAddShader(
    "textured_quad",
    "../../shaders/textured_quad.vert",
    nullptr,
    nullptr,
    nullptr,
    "../../shaders/textured_quad.frag");

  _phong_renderer = std::make_unique<Renderer>(*ShaderManager::instance().getShader("simple_shader_program"));
  _fbo_renderer = std::make_unique<Renderer>(*ShaderManager::instance().getShader("textured_quad"));

  perspective_camera.addToShader(ShaderManager::instance().getShader("simple_shader_program")->id());
  perspective_camera.addToShader(ShaderManager::instance().getShader("simple_white")->id());
  
  viewspace_ortho_camera.addToShader(ShaderManager::instance().getShader("simple_shader_program")->id());
  viewspace_ortho_camera.addToShader(ShaderManager::instance().getShader("simple_white")->id());
  viewspace_ortho_camera.addToShader(ShaderManager::instance().getShader("textured_quad")->id());

  _box = std::make_unique<RenderableBox>();
  _grid0 = std::make_unique<Grid>(ShaderManager::instance().getShader("simple_white"));
  _grid1 = std::make_unique<Grid>(ShaderManager::instance().getShader("simple_white"));
  _grid2 = std::make_unique<Grid>(ShaderManager::instance().getShader("simple_white"));
  _fbo_quad = std::make_unique<FrameBufferQuad>(720 * 2, 480 * 2);

  _grid0->setTransform(glm::rotate(glm::mat4(1.0f), static_cast<float>(M_PI_2), glm::vec3(1.0f, 0.0f,0.0f)) * glm::scale(glm::vec3(10.0f)));
  _grid1->setTransform(glm::rotate(glm::mat4(1.0f), static_cast<float>(M_PI_2), glm::vec3(1.0f, 0.0f,0.0f)) * glm::scale(glm::vec3(20.0f)));
  _grid2->setTransform(glm::rotate(glm::mat4(1.0f), static_cast<float>(M_PI_2), glm::vec3(1.0f, 0.0f,0.0f)) * glm::scale(glm::vec3(40.0f)));

  scene.addChild(*_grid0.get());
  scene.addChild(*_grid1.get());
  scene.addChild(*_grid2.get());
  scene.addChild(*_box.get());

  view_space.addChild(*_fbo_quad.get());
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update()
{
  updateTransforms();

  // Then render
  _fbo_quad->bindFBO();
  glViewport(0,0, _fbo_quad->width(), _fbo_quad->height());
  glClearColor(0.0, 0.0, 0.0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  perspective_camera.execute();
  _phong_renderer->render(scene);
  _fbo_quad->unbindFBO();

  glViewport(0,0, windowWidth(), windowHeight());
  glDisable(GL_DEPTH_TEST);
  viewspace_ortho_camera.execute();
  _fbo_renderer->render(view_space);
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  SphericalController controller(e.camera());
  WindowSizeController window_controller(e);
  window.addController(controller);
  window.addController(window_controller);
  std::function<void(void)> loop = [&]()
  {
    e.update();
  };
  
  window.run(loop);

  return 0;
}
