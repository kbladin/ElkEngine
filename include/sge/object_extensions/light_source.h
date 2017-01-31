#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/new_mesh.h"
#include "sge/core/camera.h"

#include <vector>
  
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace sge { namespace core {

class DeferredShadingRenderer;

class LightSource : public Object3D
{
public:
  LightSource(glm::vec3 color, float intensity);
  ~LightSource() {};
  virtual void submit(DeferredShadingRenderer& renderer) override;
  virtual void render(const PerspectiveCamera& camera);

  void setIntensity(float intensity);
  void setColor(glm::vec3 color);
private:
  void renderQuad(const PerspectiveCamera& camera);
  void renderSphere(const PerspectiveCamera& camera);
  void setupLightSourceUniforms(const PerspectiveCamera& camera);

  std::shared_ptr<NewMesh> _quad_mesh;
  std::shared_ptr<NewMesh> _sphere_mesh;

  glm::vec3 _color;
  float _intensity;
  float _sphere_scale;
};

} }
