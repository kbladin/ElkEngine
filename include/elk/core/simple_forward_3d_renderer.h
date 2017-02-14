#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/camera.h"
#include "elk/core/renderer.h"

#include <memory>
#include <vector>

namespace elk { namespace core {

class Renderable;

class SimpleForward3DRenderer : public Renderer {
public:
  SimpleForward3DRenderer(
    PerspectiveCamera& camera, int window_width, int window_height);
  ~SimpleForward3DRenderer();

  virtual void render(Object3D& scene) override;
private:
};

} }
