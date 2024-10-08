#ifndef SCENEPLAY_HPP
#define SCENEPLAY_HPP

#include <SFML/Graphics.hpp>

#include "Scene.hpp"
#include "Entity.hpp"
#include "EntityManager.hpp"
#include "Vec2.hpp"

struct PlayerConfig
{
    float x, y, cX, cY, speed, maxSpeed, jump, gravity;
    std::string weapon;
};

struct DecTileConfig
{
    std::string name;
    float x, y;
};

class ScenePlay: public Scene
{
public:
    ScenePlay(GameEngine* gameEngine, std::string  levelPath);
    ~ScenePlay() override = default;

    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void doAction(const Action& action) override;

    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity>& entity);
    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity>& entity);

    void sMovement();
    void sLifespan();
    void sCollision();
    void sAnimation();

protected:
    void onEnd() override;

    void init(const std::string& levelPath);
    void loadLevel(const std::string& filename);
    void showBrickCollision(const std::shared_ptr<Entity>& entity);
    void showQuestionCollision(std::shared_ptr<Entity>& entity);

    std::shared_ptr<Entity> m_player;
    std::string m_levelPath;
    PlayerConfig m_playerConfig;
    DecTileConfig m_dtConfig;

    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const Vec2 m_gridSize = {64.0f, 64.0f};
    sf::Text m_gridText;

private:
    void m_createEntity(const DecTileConfig& tile, const std::string& type);
    void debugMessage(const std::string& message) const;
    EntityManager m_entityManager;
};

#endif //SCENEPLAY_HPP
