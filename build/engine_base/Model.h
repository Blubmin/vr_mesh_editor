#pragma once

#include <string>
#include <vector>

#include <assimp\scene.h>

#include "Component.h"
#include "Material.h"
#include "Mesh.h"

class Model : public Component
{
public:
    Model();
    Model(const aiScene* scene);
    ~Model();

    std::vector<Mesh> meshes;
    std::vector<Material> materials;

private:
    void shiftUpToGround();
    void shrink();
};

