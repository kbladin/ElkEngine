#include "SGE/core/object_3d.h"

namespace sge { namespace core {

void Object3D::addChild(Object3D& child)
{
  _children.push_back(&child);
}

void Object3D::removeChild(Object3D& child)
{
  _children.erase(std::remove(
    _children.begin(), _children.end(), &child), _children.end());
  for (int i=0; i<_children.size(); i++) {
    _children[i]->removeChild(child);
  }
}

void Object3D::updateTransform(const glm::mat4& stacked_transform)
{
  _absolute_transform = stacked_transform * _relative_transform;
  for (auto it = _children.begin(); it != _children.end(); it++) {
    (*it)->updateTransform(_absolute_transform);
  }
}

void Object3D::execute()
{
  for (auto it = _children.begin(); it != _children.end(); it++) {
    (*it)->execute();
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

} }