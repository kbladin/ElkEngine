#include "elk/core/object_3d.h"

#include "elk/core/deferred_shading_renderer.h"
#include "elk/object_extensions/light_source.h"
#include "elk/core/camera.h"

namespace elk { namespace core {

void Object3D::addChild(Object3D& child)
{
  _children.push_back(&child);
}

void Object3D::removeChild(Object3D& child)
{
  _children.erase(std::remove(
    _children.begin(), _children.end(), &child), _children.end());
  for (auto ch : _children) {
    ch->removeChild(child);
  }
}

void Object3D::updateTransform(const glm::mat4& stacked_transform)
{
  _absolute_transform = stacked_transform * _relative_transform;
  for (auto ch : _children) {
    ch->updateTransform(_absolute_transform);
  }
}

void Object3D::submit(Renderer& renderer)
{
  for (auto ch : _children) {
    ch->submit(renderer);
  }
}

void Object3D::update(double dt)
{
  for (auto ch : _children) {
    ch->update(dt);
  }
}

const glm::mat4& Object3D::relativeTransform() const
{
  return _relative_transform;
}

const glm::mat4& Object3D::absoluteTransform() const
{
  return _absolute_transform;
}

void Object3D::setTransform(const glm::mat4& transform)
{
  _relative_transform = transform;
}

void RenderableDeferred::submit(Renderer& renderer)
{
  Object3D::submit(renderer);
  renderer.submitRenderableDeferred(*this);
}

void RenderableForward::submit(Renderer& renderer)
{
  Object3D::submit(renderer);
  renderer.submitRenderableForward(*this);
}

} }
