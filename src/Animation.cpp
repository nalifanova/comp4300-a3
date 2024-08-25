#include "Animation.hpp"

Animation::Animation(const std::string& name, const sf::Texture& t):
    Animation(name, t, 1, 0) {}

Animation::Animation(std::string name, const sf::Texture& t, const size_t frameCount, const size_t speed):
    m_name(std::move(name)), m_sprite(t), m_frameCount(frameCount), m_speed(speed)
{
    m_size = Vec2(static_cast<float>(t.getSize().x) / frameCount, static_cast<float>(t.getSize().y));
    m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
    m_sprite.setTextureRect(
        sf::IntRect(std::floor(m_currentFrame) * m_size.x,
                    0,
                    static_cast<int>(m_size.x),
                    static_cast<int>(m_size.y)
            )
        );
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update()
{
    // add the speed variable to the current frame
    m_currentFrame++;

    // TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
    //       2) set the texture rectangle properly (see constructor for sample)
    size_t frame = m_currentFrame / m_speed;
    frame %= m_frameCount;

    m_sprite.setTextureRect(sf::IntRect(
        frame * m_size.x,
        0,
        static_cast<int>(m_size.x),
        static_cast<int>(m_size.y)
        )
    );
}

bool Animation::hasEnded() const
{
    // TODO: detect when animation has ended (last frame was played) and return true
    if (m_frameCount == 1) return false;

    const size_t frame = m_currentFrame / m_speed;
    return frame > m_frameCount;
}

const std::string& Animation::getName() const { return m_name; }

const Vec2& Animation::getSize() const { return m_size; }

sf::Sprite& Animation::getSprite() { return m_sprite; }
