#include "Component.h"



Component::Component(unsigned long mask)
{
    this->mask = mask;
}

Component::Component() : Component(0)
{
}

Component::~Component()
{
}
