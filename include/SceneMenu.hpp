#ifndef SCENEMENU_HPP
#define SCENEMENU_HPP

#include "Scene.hpp"

class SceneMenu: public Scene
{
public:
    explicit SceneMenu(GameEngine* gameEngine = nullptr);
    void sRender() override;

protected:
    void init();
    void update() override;
    void sDoAction(const Action& action) override;
    void doAction(const Action& action) override;
    void onEnd() override;

    void sUserInput();

    std::string m_title;
    std::vector<std::string> m_menuStrings;
    std::vector<std::string> m_levelPaths;
    sf::Text m_menuText;
    size_t m_selectedMenuIndex = 0;
};

#endif //SCENEMENU_HPP
