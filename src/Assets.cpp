#include "Assets.hpp"

#include <cassert>
#include <fstream>

#include "FileUtils.hpp"

void Assets::loadFromFile(const std::string& path)
{
    const auto filePath = getFullPath(path);
    std::cout << "Contig file: " << filePath << "\n";

    std::ifstream fin(filePath);
    std::string token = "none";

    while (fin >> token)
    {
        if (token == "Texture")
        {
            fin >> m_texConf.name >> m_texConf.filepath;
            addTexture(m_texConf.name, m_texConf.filepath);
        }
        else if (token == "Animation")
        {
            fin >> m_animConf.animName >> m_animConf.texName >> m_animConf.frameCount >> m_animConf.animSpeed;
            addAnimation(m_animConf.animName, m_animConf.texName, m_animConf.frameCount, m_animConf.animSpeed);
        }
        else if (token == "Font")
        {
            fin >> m_fontConf.name >> m_fontConf.filepath;
            addFont(m_fontConf.name, m_fontConf.filepath);
        }
    }
}

void Assets::addTexture(const std::string& name, const std::string& path, const bool smooth)
{
    m_textures[name] = sf::Texture();

    if (!m_textures[name].loadFromFile(getFullPath(path)))
    {
        std::cerr << "Could not load texture from: " << path << "\n";
        m_textures.erase(name);
    }
    else
    {
        m_textures[name].setSmooth(smooth);
        std::cout << "Loaded texture: " << path << "\n";
    }
}

void Assets::addAnimation(const std::string& animName, const std::string& texName, const size_t& frameCount
                        , const size_t& speed)
{
    m_animations[animName] = Animation(animName, getTexture(texName), frameCount, speed);
}

void Assets::addFont(const std::string& name, const std::string& path)
{
    m_fonts[name] = sf::Font();
    if (!m_fonts[name].loadFromFile(getFullPath(path)))
    {
        std::cerr << "Could not load from file: " << path << "\n";
        m_fonts.erase(name);
    }
    else
    {
        std::cout << "Loaded font: " << path << "\n";
    }
}

const sf::Texture& Assets::getTexture(const std::string& name) const
{
    if (!m_textures.contains(name))
    {
        std::cerr << "There's no texture with name: " << name << "\n";
        exit(-1);
    }

    return m_textures.at(name);
}

Animation& Assets::getAnimation(const std::string& name)
{
    if (!m_animations.contains(name)) { std::cerr << "There's no animation with name: " << name << "\n"; }

    return m_animations.at(name);
}

const sf::Font& Assets::getFont(const std::string& name) const
{
    if (!m_fonts.contains(name))
    {
        std::cerr << "There is no font with name: " << name << "\n";
        exit(-1);
    }
    return m_fonts.at(name);
}
