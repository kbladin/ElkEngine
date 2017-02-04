#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/mesh.h"
#include "sge/core/texture.h"
#include "sge/core/material.h"

namespace sge { namespace core {

class RenderableModel : public Renderable
{
public:
    RenderableModel(
    	std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    ~RenderableModel(){};
    virtual void render() override;
    virtual void update(double dt) override;
private:
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Material> _material;
};

} }
