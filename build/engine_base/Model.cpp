#include "Model.h"

#include <iostream>

using namespace std;

Model::Model() : Component(COMPONENT_MODEL)
{
}

Model::Model(const aiScene* scene) : Component(COMPONENT_MODEL)
{
    meshes = vector<Mesh>();
    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        Mesh mesh = Mesh(scene->mMeshes[i]);
        meshes.push_back(mesh);
    }

    shiftUpToGround();
    shrink();

    for (int i = 0; i < this->meshes.size(); i++) {
        this->meshes[i].bindBuffers();
    }
        
    materials = vector<Material>();
    for (int i = 0; i < scene->mNumMaterials; i++)
    {
        Material material = Material(scene->mMaterials[i]);
        materials.push_back(material);
    }
}

void Model::shiftUpToGround()
{
    float miny = FLT_MAX;
    for (int i = 0; i < this->meshes.size(); i++)
    {
        for (int j = 0; j < this->meshes[i].verts.size(); j++)
        {
            miny = std::min(miny, this->meshes[i].verts[j].y);
        }
    }
    
    for (int i = 0; i < this->meshes.size(); i++)
    {
        for (int j = 0; j < this->meshes[i].verts.size(); j++)
        {
            this->meshes[i].verts[j].y -= miny;
        }
    }
}

void Model::shrink()
{
    float minx = FLT_MAX;
    float maxx = FLT_MIN;
    float miny = FLT_MAX;
    float maxy = FLT_MIN;
    float minz = FLT_MAX;
    float maxz = FLT_MIN;
    for (int i = 0; i < this->meshes.size(); i++)
    {
        for (int j = 0; j < this->meshes[i].verts.size(); j++)
        {
            maxx = std::max(maxx, this->meshes[i].verts[j].x);
            maxy = std::max(maxy, this->meshes[i].verts[j].y);
            maxz = std::max(maxz, this->meshes[i].verts[j].z);
            minx = std::min(minx, this->meshes[i].verts[j].x);
            miny = std::min(miny, this->meshes[i].verts[j].y);
            minz = std::min(minz, this->meshes[i].verts[j].z);
        }
    }

    float max_diff = FLT_MIN;
    max_diff = std::max(max_diff, maxx - minx);
    max_diff = std::max(max_diff, maxy - miny);
    max_diff = std::max(max_diff, maxz - minz);

    for (int i = 0; i < this->meshes.size(); i++)
    {
        for (int j = 0; j < this->meshes[i].verts.size(); j++)
        {
            this->meshes[i].verts[j] /= max_diff;
        }
    }
}

Model::~Model()
{
}