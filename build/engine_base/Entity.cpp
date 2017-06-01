#include "Entity.h"

using namespace std;

Entity::Entity()
{
    this->mask = 0;
    this->components = map<unsigned long, shared_ptr<Component>>();
}

Entity::~Entity()
{
}

void Entity::addComponent(std::shared_ptr<Component> component)
{
    mask |= component->mask;
    this->components[component->mask] = component;
}

shared_ptr<Component> Entity::getComponent(unsigned long mask)
{
    return this->components.at(mask);
}

bool Entity::check_mask(unsigned long mask) {
    return (this->mask & mask) == mask;
}
