#include "ModelLoader.h"

#include <vector>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glm\glm.hpp>

using namespace std;
using namespace glm;

ModelLoader::ModelLoader()
{
    this->models = map<string, shared_ptr<Model>>();
}

ModelLoader::~ModelLoader()
{

}

shared_ptr<Model> ModelLoader::loadModelByName(const string& filename, string key)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(filename,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals
        );
     
    shared_ptr<Model> model = make_shared<Model>(Model(scene));
    this->models[key] = model;
    return model;
}

shared_ptr<Model> ModelLoader::getModelByName(string key)
{
    return this->models.at(key);
}