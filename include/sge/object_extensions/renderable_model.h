#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/new_mesh.h"
#include "sge/core/texture.h"

namespace sge { namespace core {

class RenderableModel : public Renderable
{
public:
    RenderableModel(const char* mesh_path);
    ~RenderableModel(){};
    virtual void render() override;
private:
    std::shared_ptr<NewMesh> _new_mesh;
    std::shared_ptr<Texture> _tex;
};

} }