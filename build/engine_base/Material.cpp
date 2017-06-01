#include "Material.h"

using namespace glm;

Material::Material(aiMaterial * mat)
{
    aiColor3D diffuse = aiColor3D(0.06f, 0.06f, 0.06f);
    mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
    this->diffuse = vec3(diffuse.r, diffuse.g, diffuse.b);

    aiColor3D ambient = diffuse * 0.1f;
    mat->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
    this->ambient = vec3(ambient.r, ambient.g, ambient.b);

    aiColor3D specular = aiColor3D(1.0f, 1.0f, 1.0f);
    mat->Get(AI_MATKEY_COLOR_SPECULAR, specular);
    this->specular = vec3(specular.r, specular.g, specular.b);

    this->shininess = 100.f;
    mat->Get(AI_MATKEY_SHININESS, this->shininess);
}

Material::~Material()
{
}
