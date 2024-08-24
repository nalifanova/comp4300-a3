#include "GameEngine.hpp"

#include <iostream>

#include "Assets.hpp"
#include "SceneMenu.hpp"

GameEngine::GameEngine(const std::string& path) { init(path); }

// protected methods
void GameEngine::init(const std::string& path)
{
    m_assets.loadFromFile(path);

    m_window.create(sf::VideoMode(1280, 768), "Definitely Not Mario");
    m_window.setFramerateLimit(60);

    changeScene("Menu", std::make_shared<SceneMenu>(this));
}

void GameEngine::update()
{
    sUserInput();
    m_sceneMap.at(m_currentScene)->update();
    m_currentFrames++;
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) { quit(); }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::X)
            {
                std::cout << "screenshot saved to " << "test.png" << std::endl;

                sf::Texture texture;
                texture.create(m_window.getSize().x, m_window.getSize().y);
                texture.update(m_window);

                if (texture.copyToImage().saveToFile("test.png"))
                {
                    std::cout << "Screenshot saved to " << "test.png" << std::endl;
                }
            }
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            // if the current scene does not have an action associated with this key, skip the event
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
            {
                continue;
            }

            // determine start or end action by whether it was key pres or release
            const std::string actionType = (event.type == sf::Event::KeyPressed) ? "Start" : "End";

            // look up the action and send the action to the scene
            currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
        }
    }
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_sceneMap[m_currentScene];
}


// public methods
void GameEngine::changeScene(const std::string& name, const std::shared_ptr<Scene>& scene, bool endCurrentScene)
{
    m_currentScene = name;
    m_sceneMap[name] = scene;
}

std::shared_ptr<Scene> GameEngine::getScene(const std::string& name) { return m_sceneMap[name]; }

bool GameEngine::isRunning() const { return m_running && m_window.isOpen(); }

void GameEngine::quit() { m_running = false; }

void GameEngine::run()
{
    while (isRunning())
    {
        update();
    }
}

sf::RenderWindow& GameEngine::window() { return m_window; }

 Assets& GameEngine::assets() { return m_assets; }

