#pragma once

#include "glm\glm.hpp"

#include "Component.h"

class Position : public Component
{
public:
    Position();
    Position(float x, float y, float z);
    Position(glm::vec3 pos);
    ~Position();

    glm::vec3 position;
};

