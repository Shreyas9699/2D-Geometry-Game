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

    int winWidth, winHeight, FPS;

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
    e->cTrasnform = std::make_shared<CTransform>(Vec2(), Vec2(), );
    e->cShape = std::make_shared<CShape>();
    e->cInput = std::make_shared<CInput>();
    m_player = e;
}


void Game::spawnEnemy()
{
    /* Enemy is spawned properly witht he m_enemyConfig variable
       The enemy must be spawned completely within the bounds of the window */

    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnSmallEnemy(ptr<Entity> e)
{
    /*Spawn small enemies at the loc of the input enemy e
    When we create smaller enemy, we have to read the values of the original enemy
        - spawn a number of smaller enimies equal to the vertices of the original enemy
        - set eacg small enemy to the same color or org, half the size
        - small enimies are woth double points of the original enemy */

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
        playerVelocity.x -= m_playerConfig.S;
    }
    if (m_player->cInput->right)
    {
        playerVelocity.x += m_playerConfig.S;
    }
    if (m_player->cInput->up)
    {
        playerVelocity.y += m_playerConfig.S;
    }
    if (m_player->cInput->down)
    {
        playerVelocity.y -= m_playerConfig.S;
    }

    m_player->cTrasnform->velocity = playerVelocity;
    
    // to update all entities position
    for (auto& e : m_entities.getEntities())
    {
        e->cTrasnform->pos += e->cTrasnform->velocity;
    }
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
    m_window.display();
}

void Game::sEnemySpawner()
{
    // user m_currentFrame - m_lastEnemySpawnTime to determine how long it has been
    // since last enemy spawned
}

