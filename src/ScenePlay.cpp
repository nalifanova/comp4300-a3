#include "ScenePlay.hpp"

#include <fstream>
#include <map>

#include "Action.hpp"
#include "Assets.hpp"
#include "Components.hpp"
#include "FileUtils.hpp"
#include "GameEngine.hpp"
#include "Physics.hpp"
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
    return {gridX  + size.x / 2.0f, height() - gridY  - size.y / 2.0f};
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
            m_createEntity(m_dtConfig, token);
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
    m_player = m_entityManager.addEntity("Player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(
        m_playerConfig.x * m_gridSize.x, m_playerConfig.y * m_gridSize.y, m_player));
    m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.cX, m_playerConfig.cY));
    m_player->addComponent<CGravity>(m_playerConfig.gravity);
    m_player->addComponent<CInput>();
    m_player->addComponent<CState>();
}

void ScenePlay::spawnBullet(std::shared_ptr<Entity>& entity)
{
    auto& transf = entity->getComponent<CTransform>();
    auto bullet = m_entityManager.addEntity("Bullet");
    bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Buster"), true);

    // for bullet position regarding player's hand and movement direction
    float sign = transf.scale.x > 0 ? 1.0f : -1.0f;
    bullet->addComponent<CTransform>(transf.pos + Vec2(m_gridSize.x / 2.0f, 0.0f) * sign);
    bullet->addComponent<CBoundingBox>(Vec2(40, 40));
    bullet->addComponent<CLifeSpan>(3 * 60, m_currentFrame);
    bullet->getComponent<CTransform>().velocity.x = m_playerConfig.speed * sign;
}

void ScenePlay::update()
{
    m_entityManager.update();
    if (!m_paused)
    {
        sMovement();
        sLifespan();
        sCollision();
        sAnimation();
        m_currentFrame++;
    }
    sRender();
}

void ScenePlay::sMovement()
{
    auto& transf = m_player->getComponent<CTransform>();
    auto& input = m_player->getComponent<CInput>();
    auto& state = m_player->getComponent<CState>();
    Vec2 playerVelocity(0.0f, m_playerConfig.speed);

    if (input.up)
    {
        if (input.canJump)
        {
            state.state = "jumping";
            state.isInAir = true;
            playerVelocity.y = -m_playerConfig.jump; //* m_playerConfig.speed;
        }
    }
    else if (input.right)
    {
        if (!state.isInAir) { state.state = "running"; }
        playerVelocity.x = m_playerConfig.speed;
        if (transf.scale.x < 0) { transf.scale.x = 1; } // flipping to the right direction
    }
    else if (input.left)
    {
        if (!state.isInAir) { state.state = "running"; }
        playerVelocity.x = -m_playerConfig.speed;
        if (transf.scale.x > 0) { transf.scale.x = -1; } // flipping to the left direction
    }
    else if (input.shoot)
    {
        state.state = "shooting";
    }
    else if (input.slit)
    {
        state.state = "slitting";
        playerVelocity.y = m_playerConfig.speed;
    }
    else
    {
        if (!state.isInAir) { state.state = "standing"; }
    }
    transf.velocity = playerVelocity;

    for (const auto& el: m_entityManager.getEntities())
    {
        auto& t = el->getComponent<CTransform>();
        t.prevPos = t.pos;

        if (el->hasComponent<CGravity>())
        {
            el->getComponent<CTransform>().velocity.y += el->getComponent<CGravity>().gravity; //e.g.
            // NOTE: if the player is moving faster than max speed in any direction,
            //      set its speed in that direction to the max speed
        }
        t.pos += t.velocity;
    }
    // TODO: Implement the maximum player speed in both X and Y directions
}

void ScenePlay::sCollision()
{
    // Bullet and Tile
    for (auto& bullet: m_entityManager.getEntities("Bullet"))
    {
        for (auto& tile: m_entityManager.getEntities("Tile"))
        {
            const auto overlap = Physics::getOverLap(bullet, tile);
            if (overlap.x > 0.0f && overlap.y > 0.0f)
            {
                const auto prevOverlap = Physics::getPreviouslyOverlap(bullet, tile);
                if (prevOverlap.y >= 0)
                {
                    if (tile->getComponent<CAnimation>().animation.getName() == "Brick") { showBrickCollision(tile); }
                    bullet->destroy();
                }
            }
        }
    }

    // ---------------- Player and etc
    auto& plTransf = m_player->getComponent<CTransform>();
    auto& plBBox = m_player->getComponent<CBoundingBox>();

    // Player and Tile
    for (auto& el: m_entityManager.getEntities("Tile"))
    {
        const auto overlap = Physics::getOverLap(m_player, el);
        if (overlap.x > 0.0f && overlap.y > 0.0f) // Overlap: defining a direction
        {
            const auto prevOverlap = Physics::getPreviouslyOverlap(m_player, el);
            const auto& t = el->getComponent<CTransform>();

            if (prevOverlap.x > 0.0f) // top/bottom collision
            {
                // down
                if (plTransf.pos.y < t.pos.y)
                {
                    // Update the CState of the player to store whether it is in the air or not
                    m_player->getComponent<CState>().isInAir = false;
                    plTransf.pos.y += -overlap.y;
                }
                // up
                if (plTransf.pos.y > t.pos.y)
                {
                    plTransf.pos.y += overlap.y;
                    auto& a = el->getComponent<CAnimation>().animation;
                    if (a.getName() == "Brick") { showBrickCollision(el); }
                    if (a.getName() == "Question") { showQuestionCollision(el); }
                }
            }

            // side collision
            if (prevOverlap.y > 0.0f) { plTransf.pos.x += plTransf.pos.x < t.pos.x ? -overlap.x : overlap.x; }
        }
    }

    // Check to see if the player has falled down a hole (y > height())
    if (plTransf.pos.y > height())
    {
        plTransf.pos = gridToMidPixel(m_playerConfig.x, m_playerConfig.y, m_player);
    }
    if (plTransf.pos.y < m_gridSize.y / 2.0f) { plTransf.pos.y = plTransf.prevPos.y; }
    // Don't let the player walk off the left side of the map
    if (plTransf.pos.x < plBBox.size.x / 2.0f) { plTransf.pos.x = plBBox.size.x / 2.0f; }
}

void ScenePlay::sDoAction(const Action& action)
{
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
        else if (action.name() == "Shoot")
        {
            input.shoot = true;
            spawnBullet(m_player);
            input.canShoot = false;
        }
        else if (action.name() == "Slit") { input.slit = true; }
        else {}
    }
    else if (action.type() == "End")
    {
        if (action.name() == "Up")
        {
            input.up = false;
            input.canJump = true;
        }
        else if (action.name() == "Left") { input.left = false; }
        else if (action.name() == "Right") { input.right = false; }
        else if (action.name() == "Shoot")
        {
            input.shoot = false;
            input.canShoot = true;
        }
        else if (action.name() == "Slit") { input.slit = false; }
        else {}
    }
}

void ScenePlay::sAnimation()
{
    // set the animation of the player based on its CState component
    const auto& state = m_player->getComponent<CState>();
    std::map<std::string, std::string> states = {
        {"jumping", "Air"}, {"running", "Run"}, {"standing", "Stand"}, {"shooting", "Shoot"}, {"slitting", "Slit"}
    };
    for (const auto& [s, animation]: states)
    {
        if (state.state == s)
            m_player->addComponent<CAnimation>(m_game->assets().getAnimation(animation), true);
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

void ScenePlay::sLifespan()
{
    for (const auto& el: m_entityManager.getEntities())
    {
        if (!el->hasComponent<CLifeSpan>()) { continue; }
        const auto& life = el->getComponent<CLifeSpan>();
        if (life.lifespan + life.frameCreated < m_currentFrame) { el->destroy(); }
    }
}

void ScenePlay::doAction(const Action& action)
{
    sDoAction(action);
}

void ScenePlay::showBrickCollision(const std::shared_ptr<Entity>& entity)
{
    entity->getComponent<CAnimation>().animation = m_game->assets().getAnimation("Explosion");
    entity->addComponent<CLifeSpan>(60, m_currentFrame);
}

void ScenePlay::showQuestionCollision(std::shared_ptr<Entity>& entity)
{
    auto coin = m_entityManager.addEntity("Tile");
    coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
    coin->addComponent<CTransform>(
        Vec2(entity->getComponent<CTransform>().pos.x,
             entity->getComponent<CTransform>().pos.y - m_gridSize.y
            )
        );
    coin->addComponent<CLifeSpan>(30, m_currentFrame);
    // replace Question by inective
    auto q2 = m_entityManager.addEntity("Tile");
    q2->addComponent<CAnimation>(m_game->assets().getAnimation("Question2"), true);
    q2->addComponent<CTransform>(entity->getComponent<CTransform>().pos);
    q2->addComponent<CBoundingBox>(m_gridSize);
    entity->destroy();
}

// private methods
void ScenePlay::m_createEntity(const DecTileConfig& tile, const std::string& type)
{
    // Tiles have GX Grid X Pos & GY Grid Y Pos
    auto entity = m_entityManager.addEntity(type);
    entity->addComponent<CAnimation>(m_game->assets().getAnimation(tile.name), true);
    entity->addComponent<CTransform>(gridToMidPixel(tile.x * m_gridSize.x, tile.y * m_gridSize.y, entity));

    const auto& anim = entity->getComponent<CAnimation>().animation;
    auto& transf = entity->getComponent<CTransform>();

    if (anim.getSize().x < m_gridSize.x) // only for small tiles
    {
        transf.scale.x = m_gridSize.x / anim.getSize().x;
        transf.scale.y = m_gridSize.y / anim.getSize().y;
        transf.pos.x += 24.0f; // hack
        transf.pos.y -= 24.0f;
        entity->addComponent<CBoundingBox>(Vec2(m_gridSize));
    }
    else
    {
        entity->addComponent<CBoundingBox>(anim.getSize());
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
