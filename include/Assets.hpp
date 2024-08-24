#ifndef ASSETS_HPP
#define ASSETS_HPP

#include <iostream>
#include <map>

#include "Animation.hpp"

struct TexConf
{
    std::string name, filepath;
};

struct FontConf
{
    std::string name, filepath;
};

struct AnimConf
{
    std::string animName, texName;
    int frameCount, animSpeed;
};

class Assets
{
public:
    Assets() = default;
    ~Assets() = default;

    void loadFromFile(const std::string& path);

    [[nodiscard]] const sf::Texture& getTexture(const std::string& name) const;
    [[nodiscard]]  Animation& getAnimation(const std::string& name) ;
    [[nodiscard]] const sf::Font& getFont(const std::string& name) const;

private:
    void addTexture(const std::string& name, const std::string& path, bool smooth = true);
    void addAnimation(const std::string& animName, const std::string& texName, const size_t& frameCount
                    , const size_t& speed);
    void addFont(const std::string& name, const std::string& path);

    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, Animation> m_animations;
    std::map<std::string, sf::Font> m_fonts;

    TexConf m_texConf;
    AnimConf m_animConf;
    FontConf m_fontConf;

    friend class GameEngine;
};

#endif //ASSETS_HPP
