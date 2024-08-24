#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include "Entity.hpp"

#include <map>
#include <vector>

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{
public:
    EntityManager() = default;

    void update();

    std::shared_ptr<Entity> addEntity(const std::string& tag);
    [[nodiscard]] EntityVec& getEntities();
    [[nodiscard]] EntityVec& getEntities(const std::string& tag);
    [[nodiscard]] const EntityMap& getEntityMap();

private:
    // helper function to avoid repeated code
    static void removeDeadEntities(EntityVec& vec);

    EntityVec m_entities; // all entities
    EntityVec m_entitiesToAdd; // entities to add next update
    EntityMap m_entityMap; // map from entity tag to vectors
    size_t m_totalEntities = 0; // total entities created
};

#endif //ENTITYMANAGER_HPP
