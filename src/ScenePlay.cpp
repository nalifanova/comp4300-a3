#include "ScenePlay.hpp"

#include <fstream>
#include <map>

#include "Action.hpp"
#include "Assets.hpp"
#include "Components.hpp"
#include "FileUtils.hpp"
#include "GameEngine.hpp"
#include "SceneMenu.hpp"

ScenePlay::ScenePlay(GameEngine* gameEngine, std::string levelPath):
    Scene(gameEngine), m_levelPath(std::move(levelPath))
{
    init(m_levelPath);
}

void ScenePlay::init(const std::string& levelPath)
{
    registerAction(sf::Keyboard::P, "Pause");
    registerAction(sf::Keyboard::Escape, "Quit");
    registerAction(sf::Keyboard::T, "ToggleTexture");
    registerAction(sf::Keyboard::C, "ToggleCollision");
    registerAction(sf::Keyboard::G, "ToggleGrid");
    // TODO: Register all other gameplay Actions
    registerAction(sf::Keyboard::W, "Up");
    registerAction(sf::Keyboard::A, "Left");
    registerAction(sf::Keyboard::D, "Right");
    registerAction(sf::Keyboard::S, "Slit");
    registerAction(sf::Keyboard::Space, "Shoot");

    m_gridText.setCharacterSize(14);
    m_gridText.setFont(m_game->assets().getFont("Numbers"));
    loadLevel(levelPath);
}

Vec2 ScenePlay::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity>& entity)
{
    // pos_x + grid_x / 2 - math.ceil(anim_x / 2) + 1)
    auto size = entity->getComponent<CAnimation>().animation.getSize();
    Vec2 position{
        gridX + m_gridSize.x / 2.0f - size.x / m_gridSize.x + 1.0f
      , gridY + m_gridSize.y / 2.0f - size.y / m_gridSize.y + 1.0f
    };
    // vertical flip height() - dec.y * m_gridSize.x - m_gridSize.y
    position.y = height() - position.y;
    return position;
}

void ScenePlay::loadLevel(const std::string& filename)
{
    std::cout << "ScenePlay::LoadLevel " << filename << "\n";
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    const auto filePath = getFullPath(filename);
    std::cout << "Contig file: " << filePath << "\n";
    std::ifstream fin(filePath);
    std::string token = "none";
    m_game->assets().loadFromFile(filename);

    while (fin >> token)
    {
        if (token == "Tile" || token == "Dec")
        {
            fin >> m_dtConfig.name >> m_dtConfig.x >> m_dtConfig.y;
            m_createEntity(m_dtConfig);
        }
        // Player x, y, cX, cY, speed, maxSpeed, jump, gravity;
        else if (token == "Player")
        {
            fin >> m_playerConfig.x >> m_playerConfig.y >> m_playerConfig.cX >> m_playerConfig.cY
                >> m_playerConfig.speed >> m_playerConfig.maxSpeed >> m_playerConfig.jump
                >> m_playerConfig.gravity >> m_playerConfig.weapon;
        }
    }

    spawnPlayer();
}

void ScenePlay::spawnPlayer()
{
    m_player = m_entityManager.addEntity("player");

    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(
        m_playerConfig.x * m_gridSize.x, m_playerConfig.y * m_gridSize.y, m_player));
    m_player->addComponent<CBoundingBox>(gridToMidPixel(m_playerConfig.cX, m_playerConfig.cY, m_player));

    m_player->addComponent<CGravity>(m_playerConfig.gravity);

    // TODO: be sure to add the remaining components to the player
}

void ScenePlay::spawnBullet(std::shared_ptr<Entity> entity)
{
    // TODO: this should spawn a bullet at the given entity, going in the direction
    //       the entity is facing
}

void ScenePlay::update()
{
    m_entityManager.update();
    if (!m_paused)
    {
        sMovement();
        sLifespan();
        sCollision();
        m_currentFrame++;
    }
    sAnimation();
    sRender();
}

void ScenePlay::sMovement()
{
    auto& transf = m_player->getComponent<CTransform>();
    auto& input = m_player->getComponent<CInput>();
    auto& state = m_player->getComponent<CState>();
    Vec2 playerVelocity(m_playerConfig.speed, transf.velocity.y);

    if (input.up)
    {
        if (input.canJump)
        {
            input.canJump = false;
            state.state = "jumping";
            transf.velocity.y = m_playerConfig.jump; //-3;

            if (transf.pos.y <= transf.prevPos.y)
            {
                input.up = false;
                transf.velocity.y = 0;
            }
        }
    }
    else if (input.right)
    {
        state.state = "running";
        transf.velocity = playerVelocity;
        if (transf.scale.x < 0) { transf.scale.x = 1; } // flipping to the right direction
    }
    else if (input.left)
    {
        state.state = "running";
        transf.velocity = playerVelocity;
        transf.velocity.x *= -1.0f;
        if (transf.scale.x > 0) { transf.scale.x = -1; } // flipping to the left direction
    }
    else if (input.shoot) // TODO: should be finished
    {
        state.state = "shooting";
        transf.velocity = Vec2(0.0f, 0.0f);
    }
    // else if (input.slit) // Note: Think!
    // {
    //     state.state = "slitting";
    //     transf.velocity = Vec2(0.0f, 0.0f);
    // }
    else
    {
        state.state = "standing";
        input.canJump = true;
        transf.velocity.x = 0;
    }
    // transf.pos += transf.velocity;

    for (const auto& el: m_entityManager.getEntities()) // TODO: check &
    {
        auto& t = el->getComponent<CTransform>();
        t.prevPos = t.pos;

        if (el->hasComponent<CGravity>())
        {
            el->getComponent<CTransform>().velocity.y += el->getComponent<CGravity>().gravity; //e.g.
            // NOTE: if the player is moving faster than max speed in any direction,
            //      set its speed in that direction to the max speed
            t.pos.y = transf.velocity.y;
        }
        t.pos += t.velocity;
    }

    // TODO: Implement player movement / jumping based on its CInput component
    // TODO: Implement gravity's effect on the player
    // TODO: Implement the maximum player speed in both X and Y directions

    // ** Gravity is an ecceleration of velocity
}

void ScenePlay::sCollision()
{
    // REMEBER: SFML's (0,0) position is on the TOP-LEFT corner
    //          This means jumping will have a negative y-component
    //          and gravity will have a positive y-component
    //          Also, something BELOW something else will have a y value GREATER than it
    //          Also, something ABOVE something else will have a y value LESS than it

    // TODO: Implement Physics::getOverLap() function, use it inside this funtion

    // TODO: Implement bullet / tile collision
    //       Destroy the tile if it has a Brick animation
    // TODO: Implement player / tile collisions and resolutions
    //       Update the CState component of the player to store whether it is currently
    //       on the ground or in the air. This will be used by the Animation system
    // TODO: Check to see if the player has falled down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
}

void ScenePlay::sDoAction(const Action& action)
{
    std::cout << "system do Action: " << action.name() << "\n";
    auto& input = m_player->getComponent<CInput>();

    if (action.type() == "Start")
    {
        if (action.name() == "ToggleTexture") { m_drawTextures = !m_drawTextures; }
        else if (action.name() == "ToggleCollision") { m_drawCollision = !m_drawCollision; }
        else if (action.name() == "ToggleGrid") { m_drawGrid = !m_drawGrid; }
        else if (action.name() == "Pause") { setPaused(!m_paused); }
        else if (action.name() == "Quit") { onEnd(); }

        else if (action.name() == "Up") { input.up = true; }
        else if (action.name() == "Left") { input.left = true; }
        else if (action.name() == "Right") { input.right = true; }
        else if (action.name() == "Shoot") { input.shoot = true; }
        else if (action.name() == "Slit") { input.slit = true; }
        else {}
    }
    else if (action.type() == "End")
    {
        if (action.name() == "Up") { input.up = false; }
        else if (action.name() == "Left") { input.left = false; }
        else if (action.name() == "Right") { input.right = false; }
        else if (action.name() == "Shoot") { input.shoot = false; }
        else if (action.name() == "Slit") { input.slit = false; }
        else {}
    }
}

void ScenePlay::sAnimation()
{
    // set the animation of the player based on its CState component
    const auto& state = m_player->getComponent<CState>();
    std::map<std::string, std::string> states = {
        {"jumping", "Air"}, {"running", "Run"}, {"standing", "Stand"},
        {"buster", "Buster"},{"shooting", "Shoot"},{"slitting", "Slit"}
    };
    for (const auto& [s, animation]: states)
    {
        if (state.state == s)
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation(animation));
    }

    for (const auto& el: m_entityManager.getEntities())
    {
        if (el->hasComponent<CAnimation>())
        {
            auto& a = el->getComponent<CAnimation>();
            a.animation.update();

            if (!a.repeat && a.animation.hasEnded()) { el->destroy(); }
        }
    }
}

void ScenePlay::onEnd()
{
    m_game->changeScene("Menu", std::make_shared<SceneMenu>(m_game));
}

void ScenePlay::sRender()
{
    // color the background darker so you know that the game is paused
    if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
    else { m_game->window().clear(sf::Color(50, 50, 150)); }

    // DEBUG-MESSAGE
    debugMessage("Scene play - " + m_game->currentSceneName());

    // set the viewport of the window to be centered on the player if it's fat enough right
    auto& pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
    m_game->window().setView(view);

    // draw all Entity textures / animations
    if (m_drawTextures)
    {
        for (auto& e: m_entityManager.getEntities())
        {
            auto& transform = e->getComponent<CTransform>();
            if (e->hasComponent<CAnimation>())
            {
                auto& animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(transform.angle);
                animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
                animation.getSprite().setScale(transform.scale.x, transform.scale.y);
                m_game->window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collisions bounding boxes with a rectangleshape
    if (m_drawCollision)
    {
        for (auto& e: m_entityManager.getEntities())
        {
            if (e->hasComponent<CBoundingBox>())
            {
                auto& box = e->getComponent<CBoundingBox>();
                auto& transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;

                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(transform.pos.x, transform.pos.y);
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color(255, 255, 255, 255));
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that you can easily debug
    if (m_drawGrid)
    {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0f;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x)
        {
            drawLine(Vec2(x, 0), Vec2(x, height()));
        }

        for (float y = 0.0f; y < height(); y += m_gridSize.y)
        {
            drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x)
            {
                std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
                std::string yCell = std::to_string((int)y / (int)m_gridSize.y);

                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
                m_game->window().draw(m_gridText);
            }
        }
    }

    m_game->window().display();
}

void ScenePlay::sLifespan() {}

void ScenePlay::doAction(const Action& action)
{
    sDoAction(action);
}

void ScenePlay::m_createEntity(const DecTileConfig& tile)
{
    // Tiles have GX Grid X Pos & GY Grid Y Pos
    auto entity = m_entityManager.addEntity("tile");
    entity->addComponent<CAnimation>(m_game->assets().getAnimation(tile.name), true);
    entity->addComponent<CTransform>(gridToMidPixel(tile.x * m_gridSize.x, tile.y * m_gridSize.y, entity));
    entity->addComponent<CBoundingBox>(m_game->assets().getAnimation(tile.name).getSize());

    const auto& anim = entity->getComponent<CAnimation>().animation;
    auto& transf = entity->getComponent<CTransform>();

    if (anim.getSize().x < m_gridSize.x) // only for small tiles
    {
        transf.scale.x = m_gridSize.x / anim.getSize().x;
        transf.scale.y = m_gridSize.y / anim.getSize().y;
        transf.pos.x -= 1.0f;
    }
}

void ScenePlay::debugMessage(const std::string& message) const
{
    sf::Text text;
    text.setFont(m_game->assets().getFont("Tech"));
    text.setString(message);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    text.setPosition(sf::Vector2f(
        width() - text.getGlobalBounds().width - 10,
        text.getCharacterSize() - 10));
    m_game->window().draw(text);
}


