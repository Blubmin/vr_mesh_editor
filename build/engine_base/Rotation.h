#pragma once

#include <glm\glm.hpp>

#include "Component.h"

class Rotation : public Component
{
public:
    Rotation();
    Rotation(float phi, float theta, float psi);
    ~Rotation();

    float phi;
    float theta;
    float psi;
};

