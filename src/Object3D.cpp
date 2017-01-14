#include "SGE/Object3D.h"

//! Destructor
/*!
  The _children of the Object3D is not destroyed when the Object3D is destroyed.
  The _children needs to be destroyed explicitly.
*/
Object3D::~Object3D()
{

}

//! Adds a child to the Object3D
/*!
  \param child is an Object3D pointer that will be added as child.
*/
void Object3D::addChild(Object3D *child)
{
  _children.push_back(child);
}

//! Removes a child from the list of pointers
/*!
  The call is recursive so if the parameter \param child is found among
  _children of _children it is also removed
*/
void Object3D::removeChild(Object3D *child)
{
  _children.erase(std::remove(
    _children.begin(), _children.end(), child), _children.end());
  for (int i=0; i<_children.size(); i++) {
    _children[i]->removeChild(child);
  }
}

void Object3D::update(const glm::mat4& stacked_transform)
{
  _absolute_transform = stacked_transform * _relative_transform;
  for (auto it = _children.begin(); it != _children.end(); it++) {
    (*it)->update(_absolute_transform);
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

void Object3D::setTransform(glm::mat4 transform)
{
  _relative_transform = transform;
}
