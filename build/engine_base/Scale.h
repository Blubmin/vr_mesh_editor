#pragma once
#include "Component.h"
class Scale : public Component
{
public:
    Scale();
    Scale(float factor);
    Scale(float x, float y, float z);
    ~Scale();

    float x;
    float y;
    float z;
};

