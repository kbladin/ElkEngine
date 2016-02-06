#include <gl/glew.h>
#include <gl/glfw3.h>

#include <SGE/SimpleGraphicsEngine.h>

// This class extends SimpleGraphicsEngine,
// Before initializing this object, an OpenGL context must be created
class MyEngine : public SimpleGraphicsEngine
{
public:
  MyEngine();
  ~MyEngine();

  void update();
};

MyEngine::MyEngine() : SimpleGraphicsEngine()
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
  // Initialize glfw
  if (!glfwInit())
    return -1;
  // Modern OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create a windowed mode window and its OpenGL context
  GLFWwindow* window = glfwCreateWindow(720, 480, "Example1", NULL, NULL);
  if (!window)
  {
    glfwTerminate();
    return -1;
  }
  // Make the window's context current
  glfwMakeContextCurrent(window);
  printf("%s\n", glGetString(GL_VERSION));
  
  MyEngine e;

  while (!glfwWindowShouldClose(window))
  {
    e.update();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}