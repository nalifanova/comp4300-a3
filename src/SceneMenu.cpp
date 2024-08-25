#include "SceneMenu.hpp"

#include "GameEngine.hpp"
#include "ScenePlay.hpp"

SceneMenu::SceneMenu(GameEngine* gameEngine):
    Scene(gameEngine) { init(); }

void SceneMenu::sRender()
{
    m_game->window().clear(sf::Color(52, 201, 171)); // menu bgcolor
    // drawing the game title in the top-left of the screen
    m_menuText.setString(m_title);
    m_menuText.setCharacterSize(32);
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setPosition(sf::Vector2f(width() / 2 - m_menuText.getCharacterSize() * 4, 10));
    m_game->window().draw(m_menuText);

    m_menuText.setString("A: Left    S: Down    Up: W    D: Play");
    m_menuText.setFillColor(sf::Color::Black);
    m_menuText.setCharacterSize(12);
    m_menuText.setPosition(sf::Vector2f(
            m_menuText.getCharacterSize(), height() - m_menuText.getCharacterSize() * 2)
        );
    m_game->window().draw(m_menuText);

    // render meni utems
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        constexpr int fontSize = 25;
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(fontSize);
        m_menuText.setPosition(sf::Vector2f(
                width() / 2.0f - m_menuText.getCharacterSize() * 3.5f,
                height() - m_menuText.getCharacterSize() * 5 - height() / 2.0 + i * 50
                )
            );
        if (i != m_selectedMenuIndex) { m_menuText.setFillColor(sf::Color::Black); }
        else { m_menuText.setFillColor(sf::Color::Yellow); }
        m_game->window().draw(m_menuText);
    }

    m_game->window().display();
}

// protected
void SceneMenu::init()
{
    m_game->window().setKeyRepeatEnabled(false);
    m_menuText.setFont(m_game->assets().getFont("Mario")); // check font
    m_title = "Not Mario";

    m_menuStrings.emplace_back("Level 1");
    m_levelPaths.emplace_back("level1.txt");
    m_menuStrings.emplace_back("Level 2");
    m_levelPaths.emplace_back("level2.txt");
    m_menuStrings.emplace_back("Level 3");
    m_levelPaths.emplace_back("level3.txt");

    registerAction(sf::Keyboard::W, "Up");
    registerAction(sf::Keyboard::S, "Down");
    registerAction(sf::Keyboard::D, "Play");
    registerAction(sf::Keyboard::Escape, "Quit");
}

void SceneMenu::update()
{
    m_entityManager.update();
    sRender();
}

void SceneMenu::sDoAction(const Action& action)
{
    if (action.type() == "Start")
    {
        if (action.name() == "Up")
        {
            if (m_selectedMenuIndex > 0)
                m_selectedMenuIndex--;
            else
                m_selectedMenuIndex = m_menuStrings.size() - 1;
        }
        if (action.name() == "Down")
        {
            m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
        }
        if (action.name() == "Play")
        {
            m_game->changeScene("Play", std::make_shared<ScenePlay>(
                                    m_game,
                                    m_levelPaths[m_selectedMenuIndex]
                                    )
                );
        }
        if (action.name() == "Quit") { onEnd(); }
    }
}

void SceneMenu::doAction(const Action& action)
{
    sDoAction(action);
}

void SceneMenu::onEnd() { m_game->quit(); }
