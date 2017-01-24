#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/core/simple_graphics_engine.h>
#include <SGE/window/application_window_glfw.h>
#include <SGE/core/shader_manager.h>
#include <SGE/core/new_mesh.h>
#include "sge/core/create_mesh.h"

#include <functional>
#include <memory>

using namespace sge::core;
using namespace sge::window;

class RenderableBox : public Object3D
{
public:
    RenderableBox(ShaderProgram* program);
    ~RenderableBox(){};
    virtual void execute() override;
private:
    //TriangleMesh _mesh;
    std::shared_ptr<NewMesh> _new_mesh;
    ShaderProgram* _program;
};

RenderableBox::RenderableBox(ShaderProgram* program) :
    _program(program)
{
  _new_mesh = CreateMesh::load("../../data/meshes/bunny.obj");
  //_mesh.initBox( glm::vec3(0.25, 0.25, 0.25), glm::vec3(-0.25, -0.25, -0.25));
}

void RenderableBox::execute()
{
    _program->bind();

    glUniformMatrix4fv(
      glGetUniformLocation(_program->id(), "M"),
      1,
      GL_FALSE,
      &absoluteTransform()[0][0]);

    //_mesh.render();
    _new_mesh->render();
    _program->unbind();
}

class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update();
private:
    std::unique_ptr<RenderableBox> _box;
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

  perspective_camera.addToShader(ShaderManager::instance().getShader("simple_shader_program")->id());
  viewspace_ortho_camera.addToShader(ShaderManager::instance().getShader("simple_shader_program")->id());

  _box = std::make_unique<RenderableBox>(ShaderManager::instance().getShader("simple_shader_program"));

  scene.addChild(*_box.get());
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update()
{
  render();
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  SphericalController controller(e.camera());
  window.addController(controller);
  std::function<void(void)> loop = [&]()
  {
    e.update();
  };
  
  window.run(loop);

  return 0;
}
