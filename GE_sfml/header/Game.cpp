#include "Game.h"

Game::Game(const std::string& config)
{
	init(config);
}

// need to do some changes based on the config file
void Game::init(const std::string& config)
{
    std::ifstream fin(config);

    // Check if the file is successfully opened 
    if (!fin.is_open()) {
        std::cerr << "Error opening the file!" << "\n";
        exit;
    }

    // default values
    int winWidth = 1280, winHeight = 720, FPS = 60;

    // Player input
    fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FG
        >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT
        >> m_playerConfig.V;

    /* other detail to be loaded*/


    fin.close();

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    m_window.create(sf::VideoMode(winWidth, winHeight), "SFML Game!", sf::Style::Default, settings);
    m_window.setFramerateLimit(FPS);

    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();

        sEnemySpawner();
        sMovement();
        sCollision();
        sUserInput();
        sRender();

        m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{

}

void Game::spawnPlayer()
{
    auto e = m_entities.addEntity("player");
    e->cTrasnform = std::make_shared<CTransform>(Vec2(200, 200), Vec2(1.0f, 1.0f), 0.0f);
    e->cShape = std::make_shared<CShape>(32.0f, 8, sf::Color(10, 10, 10), sf::Color(255, 0, 0), 4.0f);
    e->cInput = std::make_shared<CInput>();
    m_player = e;
}

void Game::sEnemySpawner()
{
    // user m_currentFrame - m_lastEnemySpawnTime to determine how long it has been
    // since last enemy spawned
    spawnEnemy();
}

void Game::spawnEnemy()
{
    /* Enemy is spawned properly witht he m_enemyConfig variable
       The enemy must be spawned completely within the bounds of the window */
    auto e = m_entities.addEntity("player");
    e->cTrasnform = std::make_shared<CTransform>(Vec2(100, 100), Vec2(1.0f, 1.0f), 0.0f);
    e->cShape = std::make_shared<CShape>(40.0f, 6, sf::Color(20, 120, 150), sf::Color(0, 255, 0), 5.0f);
    e->cInput = std::make_shared<CInput>();

    auto e1 = m_entities.addEntity("player");
    e1->cTrasnform = std::make_shared<CTransform>(Vec2(400, 250), Vec2(1.0f, 1.0f), 0.0f);
    e1->cShape = std::make_shared<CShape>(25.0f, 3, sf::Color(150, 10, 200), sf::Color(0, 0, 255), 5.0f);
    e1->cInput = std::make_shared<CInput>();

    auto e3 = m_entities.addEntity("player");
    e3->cTrasnform = std::make_shared<CTransform>(Vec2(600, 500), Vec2(1.0f, 1.0f), 0.0f);
    e3->cShape = std::make_shared<CShape>(25.0f, 5, sf::Color(90, 140, 230), sf::Color(0, 255, 255), 5.0f);
    e3->cInput = std::make_shared<CInput>();

    
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemy(ptr<Entity> e)
{
    //Spawn small enemies at the loc of the input enemy e
    //When we create smaller enemy, we have to read the values of the original enemy
    //    - spawn a number of smaller enimies equal to the vertices of the original enemy
    //    - set eacg small enemy to the same color or org, half the size
     //   - small enimies are woth double points of the original enemy 

    int vertices = e->cShape->circle.getPointCount();

    for (int i = 0; i < vertices; i++)
    {
        auto smallEnemy = m_entities.addEntity("small");
        smallEnemy->cScore = std::make_shared<CScore>(e->cScore->score * 2);
        // read properties of org and set it to smaller ones
    }
}

void Game::spawnBullet(ptr<Entity> e, const Vec2& mousePos)
{
    /* Bullet which travesl towards target
        - Bullet speed is given as a scalar speed
        - Must set the velocity by using formula in notes */
}

void Game::spawnSpecialWeapon(ptr<Entity> e)
{
    /*Implement special weapon when right clicked*/
}

void Game::sMovement()
{
    // should read the m_player->cInput component to determine if the player is moving
    Vec2 playerVelocity;
    if (m_player->cInput->left)
    {
        playerVelocity.x -= 5.0f;
    }
    if (m_player->cInput->right)
    {
        playerVelocity.x += 5.0f;
    }
    if (m_player->cInput->up)
    {
        playerVelocity.y += 5.0f;
    }
    if (m_player->cInput->down)
    {
        playerVelocity.y -= 5.0f;
    }

    m_player->cTrasnform->velocity = playerVelocity;
    m_player->cTrasnform->pos += m_player->cTrasnform->velocity;
    
    // to update all entities position
    //for (auto& e : m_entities.getEntities())
    //{
    //    e->cTrasnform->pos += e->cTrasnform->velocity;
    //}
}

void Game::sUserInput()
{
    // handle user input
    //  only be setting the playes input componenet variables here
    // you should not implement the playes movement logic here
    // the movement system will read the variables set in this function

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event is triggered when the close button is clicked
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }

        // event when WASD keys are pressed
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            m_player->cInput->down = true;
        }
        else
        {
            m_player->cInput->down = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            m_player->cInput->up = true;
        }
        else
        {
            m_player->cInput->up = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            m_player->cInput->left = true;
        }
        else
        {
            m_player->cInput->left = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            m_player->cInput->right = true;
        }
        else
        {
            m_player->cInput->right = false;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            m_player->cInput->shoot = true;
        }
        else
        {
            m_player->cInput->shoot = false;
        }

        // Check for collisions with window edges
        // Check for collisions with window edges
        float vx = m_player->cTrasnform->pos.x;
        float vy = m_player->cTrasnform->pos.y;
        float playerWidth = m_player->cShape->circle.getGlobalBounds().width;
        float playerHeight = m_player->cShape->circle.getGlobalBounds().height;
        int winWidth = m_window.getSize().x;
        int winHeight = m_window.getSize().y;

        // Ensure player stays within window bounds
        if (vx < 0.f) {
            vx = 0.f;
        }
        else if (vx + playerWidth > winWidth) {
            vx = winWidth - playerWidth;
        }

        if (vy < 0.f) {
            vy = 0.f;
        }
        else if (vy + playerHeight > winHeight) {
            vy = winHeight - playerHeight;
        }

        // Update player's position
        m_player->cTrasnform->pos.x = vx;
        m_player->cTrasnform->pos.y = vy;

        // event when mouce button pressed, both left and right
    }
}

void Game::sLifeSpan()
{
    /* for all entities
        - if entity has no lifespan componen, skip it if(!e->cLifeSpan)
        - if enityt has lifespan > 0, reduce by 1
        if it has lifespan and is alive
            scale its alpha channel properly
             auto color = e->cShape->circle.getFillColor();
             
             int newAlpha = 100;
             sf::Color newColor(color.r, color.g, color.b, newAlpha);
             e->cShape->circle.setFillColor(newColor);
        
        if it has lifespan and its time is up
            destory the entity*/
}

void Game::sCollision()
{

    // collision between entities, player and enemies
    // collision between enemies and bullets,
    // use collision radius not the shape radius
}

void Game::sRender()
{
    m_window.clear();

    // sample om how to draw a player, need to do same to draw emeny and bullets
    m_player->cShape->circle.setPosition(m_player->cTrasnform->pos.x, m_player->cTrasnform->pos.y);
    
    
    m_player->cTrasnform->angle += 1.0f;
    m_player->cShape->circle.setRotation(m_player->cTrasnform->angle);
    m_window.draw(m_player->cShape->circle);

    for (auto e : m_entities.getEntities())
    {
        e->cShape->circle.setPosition(e->cTrasnform->pos.x, e->cTrasnform->pos.y);
        e->cTrasnform->angle += 1.0f;
        e->cShape->circle.setRotation(m_player->cTrasnform->angle);
        m_window.draw(e->cShape->circle);
    }
    m_window.display();
}

