#include <vector>
#include <string>

#include <gl/glfw3.h>

// Function from opengl-tutorials.org
class ShaderLoader {
public:
  static GLuint loadShaders(
                     const char * vertex_file_path,
                     const char * fragment_file_path);
private:
  
};