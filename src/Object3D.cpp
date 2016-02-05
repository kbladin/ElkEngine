#include "SGE/Object3D.h"

//! Destructor
/*!
  The children of the Object3D is not destroyed when the Object3D is destroyed.
  The children needs to be destroyed explicitly.
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
  children.push_back(child);
}

//! Removes a child from the list of pointers
/*!
  The call is recursive so if the parameter \param child is found among
  children of children it is also removed
*/
void Object3D::removeChild(Object3D *child)
{
  children.erase(std::remove(children.begin(), children.end(), child), children.end());
  for (int i=0; i<children.size(); i++) {
    children[i]->removeChild(child);
  }
}

//! Calling the render function for all of the children
/*!
  \param M is the transformation matrix of the parent.
*/
void Object3D::render(glm::mat4 M)
{
  for (std::vector<Object3D*>::const_iterator iter = children.begin(); iter != children.end(); iter++) {
    (*iter)->render(M * transform_matrix_);
  }
}
