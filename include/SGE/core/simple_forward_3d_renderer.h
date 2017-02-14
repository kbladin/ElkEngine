#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/camera.h"
#include "sge/core/renderer.h"

#include <memory>
#include <vector>

namespace sge { namespace core {

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
