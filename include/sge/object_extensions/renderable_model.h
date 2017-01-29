#pragma once

#include "sge/core/object_3d.h"
#include "sge/core/new_mesh.h"
#include "sge/core/texture.h"

namespace sge { namespace core {

class RenderableModel : public Object3D
{
public:
    RenderableModel(const char* mesh_path);
    ~RenderableModel(){};
    virtual void execute() override;
private:
    std::shared_ptr<NewMesh> _new_mesh;
    std::shared_ptr<Texture> _tex;
};

} }