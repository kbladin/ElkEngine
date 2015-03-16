#include <iostream>
#include <vector>

#include <gl/glew.h>
#include <gl/glfw3.h>

#include "SimpleGraphicsEngine.h"

int main(int argc, char const *argv[])
{
  MyGraphicsEngine e;
  e.run();
  return 0;
}