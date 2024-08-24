#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <string>
#include <tuple>

#include "Components.hpp"

class EntityManager;

typedef std::tuple<CTransform, CLifeSpan, CInput, CBoundingBox, CAnimation, CGravity, CState> ComponentTable;

class Entity
{
public:
    ~Entity() = default;

    [[nodiscard]] bool isActive() const;
    [[nodiscard]] std::string tag() const;
    [[nodiscard]] size_t id() const;
    void destroy();

    template <typename T>
    [[nodiscard]] bool hasComponent() { return getComponent<T>().has; }

    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&...mArgs)
    {
        auto& component = getComponent<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.has = true;
        return component;
    }

    template <typename T>
    T& getComponent() { return std::get<T>(m_components); }

    // template <typename T>
    // const T& getComponent() const { return std::get<T>(m_components); }

    template <typename T>
    void removeComponent() { getComponent<T>() = T(); }

private:
    // constructor is private so we can never create entities
    // outside the EntityManager which had friend access
    Entity(const size_t& id, std::string  tag);

    bool m_active = true;
    size_t m_id = 0;
    std::string m_tag = "default";
    ComponentTable m_components;

    friend class EntityManager;
};

#endif //ENTITY_HPP
