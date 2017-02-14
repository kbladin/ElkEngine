#pragma once

#include "elk/core/object_3d.h"
#include "elk/core/mesh.h"
#include "elk/core/texture.h"
#include "elk/core/material.h"

namespace elk { namespace core {

class RenderableModel : public RenderableDeferred
{
public:
    RenderableModel(
    	std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material);
    ~RenderableModel(){};
    virtual void render(const UsefulRenderData& render_data) override;
    virtual void update(double dt) override;
private:
    std::shared_ptr<Mesh> _mesh;
    std::shared_ptr<Material> _material;
};

} }
