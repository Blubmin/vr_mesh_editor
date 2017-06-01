#include "Position.h"

using namespace glm;

Position::Position() : Position(0, 0, 0)
{
}

Position::Position(float x, float y, float z) : Position(vec3(x,y,z))
{
}

Position::Position(glm::vec3 pos) : Component(COMPONENT_POSITION)
{
    this->position = pos;
}

Position::~Position()
{
}
