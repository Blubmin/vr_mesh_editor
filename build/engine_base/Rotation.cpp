#include "Rotation.h"

Rotation::Rotation() : Rotation(0, 0, 0)
{

}

Rotation::Rotation(float phi, float theta, float psi) : Component(COMPONENT_ROTATION)
{
    this->phi = phi;
    this->theta = theta;
    this->psi = psi;
}


Rotation::~Rotation()
{
}
