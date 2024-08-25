#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <iostream>
#include <SFML/Graphics.hpp>

#include "Assets.hpp"
#include "Scene.hpp"

typedef std::map<std::string, std::shared_ptr<Scene>> SceneMap;

class GameEngine
{
public:
    GameEngine() = default;
    explicit GameEngine(const std::string& path);
    ~GameEngine() = default;

    void changeScene(const std::string& name, const std::shared_ptr<Scene>& scene, bool endCurrentScene = false);
    std::shared_ptr<Scene> getScene(const std::string& name);
    bool isRunning() const;
    void quit();
    void run();

    sf::RenderWindow& window();
    Assets& assets();
    [[nodiscard]] std::string currentSceneName();

protected:
    void init(const std::string& path);
    void update();
    void sUserInput();
    std::shared_ptr<Scene> currentScene();

    sf::RenderWindow m_window;
    Assets m_assets;
    std::string m_currentScene;
    SceneMap m_sceneMap;
    size_t m_simulationSpeed = 1;
    bool m_running = true;
    size_t m_currentFrames = 0;
};

#endif //GAME_ENGINE_HPP
