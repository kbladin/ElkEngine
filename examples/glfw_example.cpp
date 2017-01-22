#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/core/simple_graphics_engine.h>
#include <SGE/window/application_window_glfw.h>


#include <functional>

// This class extends SimpleGraphicsEngine,
// Before initializing this object, an OpenGL context must be created

using namespace sge::core;
using namespace sge::window;

class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update();
};

MyEngine::MyEngine() : SimpleGraphicsEngine(720, 480)
{
  
}

MyEngine::~MyEngine()
{
  
}

void MyEngine::update()
{
  SimpleGraphicsEngine::render();
}

int main(int argc, char const *argv[])
{
  ApplicationWindowGLFW window(720, 480);
  MyEngine e;
  std::function<void(void)> loop = [&]()
  {
    e.update();
  };
  
  window.run(loop);

  return 0;
}
