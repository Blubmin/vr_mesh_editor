#pragma once

#include <assimp\scene.h>
#include <glm\glm.hpp>

class Material
{
public:
    Material(aiMaterial* mat);
    ~Material();

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess; 
};

