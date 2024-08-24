#include "Entity.hpp"

Entity::Entity(const size_t& id, std::string  tag):
    m_id(id), m_tag(std::move(tag)) {}

bool Entity::isActive() const
{
    return m_active;
}

std::string Entity::tag() const
{
    return m_tag;
}

size_t Entity::id() const
{
    return m_id;
}

void Entity::destroy()
{
    m_active = false;
}

