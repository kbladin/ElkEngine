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
  _children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
  for (int i=0; i<_children.size(); i++) {
    _children[i]->removeChild(child);
  }
}

//! Calling the render function for all of the children
/*!
  \param M is the transformation matrix of the parent.
*/
void Object3D::render(glm::mat4 M)
{
  for (std::vector<Object3D*>::const_iterator iter = _children.begin(); iter != _children.end(); iter++) {
    (*iter)->render(M * transform_matrix);
  }
}
