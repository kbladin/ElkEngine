#pragma once

#include "SGE/core/controller.h"

#include <gl/glfw3.h>

#include <functional>
#include <vector>

namespace sge { namespace window {
    
using namespace core;

//! A class that handles user interfacing
class ApplicationWindowGLFW
{
public:
  ApplicationWindowGLFW(int width, int height);
  ~ApplicationWindowGLFW();

  void run(std::function<void(void)> f);
  void addController(Controller& controller);
private:
  // Functions
  bool initOpenGLContext(int width, int height);
  static void mousePosCallback(
    GLFWwindow * window,
    double x,
    double y);
  static void mouseButtonCallback(
    GLFWwindow * window,
    int button,
    int action,
    int mods);
  static void mouseScrollCallback(
    GLFWwindow * window,
    double dx,
    double dy);
  static void keyCallback(
    GLFWwindow * window,
    int key,
    int scancode,
    int action,
    int mods);
  static void windowSizeCallback(
    GLFWwindow* window,
    int width,
    int height);

  // Data
  GLFWwindow* _window;
  static std::vector<Controller*> _controllers;

  float _delay_counter;
  int   _frame_counter;
  double _time;
};

} }
