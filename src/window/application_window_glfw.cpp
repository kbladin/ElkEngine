#include "SGE/window/application_window_glfw.h"

#include <sstream>
#include <iostream>

namespace sge { namespace window {

std::vector<Controller*> ApplicationWindowGLFW::_controllers;

ApplicationWindowGLFW::ApplicationWindowGLFW(int width, int height)
{
  _frame_counter = 0;
  _delay_counter = 0;
  _time = glfwGetTime();
  // First an OpenGL context needs to be created
  if (!initOpenGLContext(width, height))
  {
    std::cout << "ERROR : Failed to initialize OpenGL" << std::endl;
  }
  glfwSwapInterval(0);
  // Set callback functions
  glfwSetCursorPosCallback(_window, mousePosCallback);
  glfwSetMouseButtonCallback(_window, mouseButtonCallback);
  glfwSetScrollCallback(_window, mouseScrollCallback);
  glfwSetKeyCallback(_window, keyCallback);
  glfwSetWindowSizeCallback(_window, windowSizeCallback);
}

ApplicationWindowGLFW::~ApplicationWindowGLFW()
{
  glfwTerminate();
}

bool ApplicationWindowGLFW::initOpenGLContext(int width, int height)
{
  // Initialize glfw
  if (!glfwInit())
    return false;
  // Modern OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Create a windowed mode window and its OpenGL context
  _window = glfwCreateWindow(width, height, "Window", NULL, NULL);
  if (!_window)
  {
    glfwTerminate();
    return false;
  }
  // Make the window's context current
  glfwMakeContextCurrent(_window);
  printf("%s\n", glGetString(GL_VERSION));
  return true;
}

//! Starts the main loop
void ApplicationWindowGLFW::run(std::function<void(double)> f)
{
  while (!glfwWindowShouldClose(_window))
  {
    double time_since_last = glfwGetTime() - _time;
    _delay_counter += time_since_last;

    if (_delay_counter >= 1.0) {
      std::stringstream title;
      title << "Window. " << _frame_counter << " FPS";
      glfwSetWindowTitle(_window, title.str().c_str());
      _frame_counter = 0;
      _delay_counter = 0;
    }
    
    for (auto&& controller : _controllers)
    {
      controller->step(time_since_last);
    }

    f(time_since_last);
    
    _time = glfwGetTime();
    _frame_counter++;
    
    //glFlush();
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}

void ApplicationWindowGLFW::addController(Controller& controller)
{
  _controllers.push_back(&controller);
  windowSizeCallback(_window, 0, 0);
}

void ApplicationWindowGLFW::mousePosCallback(
  GLFWwindow * window,
  double x,
  double y)
{
  int size_x, size_y;
  glfwGetWindowSize(window, &size_x, &size_y);
  // Convert to NDC
  x = (x / size_x - 0.5) * 2;
  y = (1 - (y / size_y) - 0.5) * 2;
  for (auto&& controller : _controllers)
  {
    controller->mousePosCallback(x, y);
  }
}

void ApplicationWindowGLFW::mouseButtonCallback(
  GLFWwindow * window,
  int button,
  int action,
  int mods)
{
  for (auto&& controller : _controllers)
  {
    controller->mouseButtonCallback(
      static_cast<MouseButton>(button), static_cast<KeyAction>(action));
  }
}

void ApplicationWindowGLFW::mouseScrollCallback(
  GLFWwindow * window,
  double dx,
  double dy)
{
  for (auto&& controller : _controllers)
  {
    controller->mouseScrollCallback(dx, dy);
  }
}

void ApplicationWindowGLFW::keyCallback(
  GLFWwindow * window,
  int key,
  int scancode,
  int action,
  int mods)
{
  for (auto&& controller : _controllers)
  {
    controller->keyCallback(
      static_cast<Key>(key), static_cast<KeyAction>(action));
  }
}

void ApplicationWindowGLFW::windowSizeCallback(
  GLFWwindow* window,
  int width,
  int height)
{
  int frame_buffer_size_x, frame_buffer_size_y;
  glfwGetFramebufferSize(window, &frame_buffer_size_x, &frame_buffer_size_y);
  for (auto&& controller : _controllers)
  {
    controller->windowSizeCallback(frame_buffer_size_x, frame_buffer_size_y);
  }
}

} }
