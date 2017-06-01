#pragma once

#include <map>
#include <memory>
#include <string>

#include "Model.h"

class ModelLoader
{
public:
    ModelLoader();
    ~ModelLoader();

    std::shared_ptr<Model> loadModelByName(const std::string& filename, std::string key);
    std::shared_ptr<Model> getModelByName(std::string key);

private:
    std::map<std::string, std::shared_ptr<Model>> models;
};

