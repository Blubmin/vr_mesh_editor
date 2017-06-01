#include "Scale.h"



Scale::Scale() : Scale(1)
{
}

Scale::Scale(float factor) : Scale(factor, factor, factor)
{
}

Scale::Scale(float x, float y, float z) : Component(COMPONENT_SCALE)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


Scale::~Scale()
{
}
