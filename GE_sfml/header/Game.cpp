#include "Game.h"
#include <ctime>

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
    // reads all necessary details about player, enemies, bullets from config files and loads it and configures the SFML window
    std::ifstream fin(config);

    // Check if the file is successfully opened 
    if (!fin.is_open()) {
        std::cerr << "Error opening the file!" << "\n";
        exit;
    }

    // default values
    int winWidth = 1280, winHeight = 720, FPS = 60, fsMode = 1;

    std::string fontFileName;
    int fontSize, fontR, fontG, fontB;

    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "Window") {
            iss >> winWidth >> winHeight >> FPS >> fsMode;
        }
        else if (type == "Font") {
            iss >> fontFileName >> fontSize >> fontR >> fontG >> fontB;
        }
        else if (type == "Player") {
            iss >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR >> m_playerConfig.FB
                >> m_playerConfig.FG >> m_playerConfig.OR >> m_playerConfig.OG >> m_playerConfig.OB >> m_playerConfig.OT
                >> m_playerConfig.V;
        }
        else if (type == "Enemy") {
            iss >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX >> m_enemyConfig.OR
                >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX
                >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        else if (type == "Bullet") {
            iss >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR >> m_bulletConfig.FG
                >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG >> m_bulletConfig.OB >> m_bulletConfig.OT
                >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }
    fin.close();


    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    if (fsMode) {
        m_window.create(sf::VideoMode(winWidth, winHeight), "My SFML Game", sf::Style::Fullscreen);
    }
    else
    {
        m_window.create(sf::VideoMode(winWidth, winHeight), "SFML Game!", sf::Style::Default, settings);
    }
    m_window.setFramerateLimit(FPS);

 
    if (!m_font.loadFromFile(fontFileName))
    {
        // if can't load the font file
        std::cerr << "Failed to load font\n";
        exit(-1);
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(fontSize);
    m_text.setString("Score : ");
    m_text.setFillColor(sf::Color(fontR, fontG, fontB));
    sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setPosition(0, 0);
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
        sLifeSpan();
        m_currentFrame++;
    }
}

void Game::setPaused(bool paused)
{

}

void Game::spawnPlayer()
{
    auto e = m_entities.addEntity("Player");
    float centerX = m_window.getSize().x / 2.0f;
    float centerY = m_window.getSize().y / 2.0f;
    e->cTransform = std::make_shared<CTransform>(Vec2(centerX, centerY), Vec2(m_playerConfig.S, m_playerConfig.S), 1.0f);
    e->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), 
                                            sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);
    e->cInput = std::make_shared<CInput>();
    m_player = e;
}

void Game::sEnemySpawner()
{
    // Users enemySpawnInterval to determine when the next emeny will be spawned
    const int enemySpawnInterval = 100;
    if (m_currentFrame >= m_lastEnemySpawnTime + enemySpawnInterval)
    {
        spawnEnemy();
    }
}

void Game::spawnEnemy()
{
    /* Enemy is spawned properly witht he m_enemyConfig variable
       The enemy must be spawned completely within the bounds of the window */
    m_lastEnemySpawnTime = m_currentFrame;
    auto e = m_entities.addEntity("Enemy");

    // Random positions with in bounds
    /* Edge case:
        The enemy object taking ran positions such that they line exactly on the edge of the window
        SOl: subtracting the range from SR to avoid getting the point that lie exactly on the screen edge */
    srand(time(NULL)); // Seed rand() with the current system time; to avoid deterministic behavior.
    float posX = 1 + m_enemyConfig.SR + (rand() % (m_window.getSize().x - 2 * m_enemyConfig.SR));
    float posY = 1 + m_enemyConfig.SR + (rand() % (m_window.getSize().y - 2 * m_enemyConfig.SR));

    // Random Vertices
    int randV = m_enemyConfig.VMIN + (rand() % (1 + m_enemyConfig.VMAX - m_enemyConfig.VMIN));

    // Randown Speed
    int randS = m_enemyConfig.SMIN + (rand() % (int)(1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN));

    // Randowm Colors
    int RGBMax = 255;
    int randR = (rand() % RGBMax);
    int randG = (rand() % RGBMax);
    int randB = (rand() % RGBMax);

    e->cTransform = std::make_shared<CTransform>(Vec2(posX, posY), Vec2(randS, randS), 1.0f);
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randV, sf::Color(randR, randG, randB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_playerConfig.OT);
    e->cLifeSpan = std::make_shared<CLifeSpan>(600); // frames is the life span of an enemy
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

    auto bulletEntity = m_entities.addEntity("Bullet");

    // calculate the anlge between player and target / mouse position where it was clicked
    Vec2 dir = { (mousePos.x - e->cTransform->pos.x), (mousePos.y - e->cTransform->pos.y) };
    float angle = atan2(dir.x, dir.y);
        
    float magnitude = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (magnitude > 0.0001f) { // Avoid division by zero
        dir.x /= magnitude;
        dir.y /= magnitude;
    };
    Vec2 newPos = e->cTransform->pos + Vec2(m_playerConfig.SR, m_playerConfig.SR) * dir;
    bulletEntity->cTransform = std::make_shared<CTransform>(newPos, Vec2(m_bulletConfig.S, m_bulletConfig.S), angle);
    bulletEntity->cTransform->velocity = dir * m_bulletConfig.S;
    bulletEntity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    bulletEntity->cLifeSpan = std::make_shared<CLifeSpan>(600);
}

void Game::spawnSpecialWeapon(ptr<Entity> e)
{
    /*Implement special weapon when right clicked*/
}

void Game::sMovement()
{
    float windowWidth = static_cast<float>(m_window.getSize().x);
    float windowHeight = static_cast<float>(m_window.getSize().y);

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

    m_player->cTransform->velocity = playerVelocity;

    // Check for collisions with window boundaries and clamp velocity
    float playerRadius = m_player->cShape->circle.getRadius(); // Assuming circle shape

    // Clamp X velocity
    if (m_player->cTransform->pos.x - playerRadius + m_player->cTransform->velocity.x < 0) {
        m_player->cTransform->velocity.x = 0.0f; // Prevent going off-screen left
    }
    else if (m_player->cTransform->pos.x + playerRadius + m_player->cTransform->velocity.x > windowWidth) {
        m_player->cTransform->velocity.x = 0.0f; // Prevent going off-screen right
    }

    // Clamp Y velocity
    if (m_player->cTransform->pos.y - playerRadius + m_player->cTransform->velocity.y < 0) {
        m_player->cTransform->velocity.y = 0.0f; // Prevent going off-screen top
    }
    else if (m_player->cTransform->pos.y + playerRadius + m_player->cTransform->velocity.y > windowHeight) {
        m_player->cTransform->velocity.y = 0.0f; // Prevent going off-screen bottom
    }
    m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
    m_player->cTransform->pos.y += m_player->cTransform->velocity.y;

    // Enemy Movement and Position Update
    for (auto& e : m_entities.getEntities("Enemy"))
    {
        // Reflect enemy at window boundaries
        if (e->cTransform->pos.x <= m_enemyConfig.SR || e->cTransform->pos.x >= windowWidth - playerRadius)
        {
            e->cTransform->velocity.x *= -1;
        }
        else if (e->cTransform->pos.y <= playerRadius || e->cTransform->pos.y >= windowHeight - playerRadius)
        {
            e->cTransform->velocity.y *= -1;
        }

        // Movement update
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    //  Bullet Position Update
    for (auto& e : m_entities.getEntities("Bullet"))
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }

    for (auto& e : m_entities.getEntities("SmallEnemy"))
    {
        e->cTransform->pos.x += e->cTransform->velocity.x;
        e->cTransform->pos.y += e->cTransform->velocity.y;
    }
    
}

void Game::sUserInput()
{
    // handle user input, move player and to be able to shoot and special weapon 

    sf::Event event;
    while (m_window.pollEvent(event))
    {
        // this event is triggered when the close button is clicked
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }

        // event when WASD keys are pressed
        // event when WASD keys are pressed
        m_player->cInput->down = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
        m_player->cInput->up = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
        m_player->cInput->left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
        m_player->cInput->right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

        // event when mouse button pressed, both left and right
        if (event.type == sf::Event::MouseButtonPressed)
        {
            if (event.mouseButton.button == sf::Mouse::Left)
            {
                // Spawn a bullet at the mouse click position
                Vec2 target(event.mouseButton.x, event.mouseButton.y);
                spawnBullet(m_player, target);
            }
        }

        // event when mouce button pressed, both left and right
    }
}

void Game::sLifeSpan()
{
    /* This code calcuates the lispane of entites such as Bullet and Enemy
            and apply fade effect as they come close to death */

    double fadeQty = 0.0000005;
    for (auto& e : m_entities.getEntities("Enemy"))
    {
        if (e->cLifeSpan->remainingLife > 0)
        {
            e->cLifeSpan->remainingLife--;
            if (e->cLifeSpan->remainingLife < e->cLifeSpan->total / 2)
            {
                auto color = e->cShape->circle.getFillColor();
                color.a -= fadeQty;
                e->cShape->circle.setFillColor(color);
                color = e->cShape->circle.getOutlineColor();
                color.a -= fadeQty;
                e->cShape->circle.setOutlineColor(color);
            }
        }
        else
        {
            e->destory();
        }
    }

    for (auto& e : m_entities.getEntities("Bullet"))
    {
        if (e->cLifeSpan->remainingLife > 0)
        {
            e->cLifeSpan->remainingLife--;
            auto color = e->cShape->circle.getFillColor();
            color.a -= fadeQty;
            e->cShape->circle.setFillColor(color);
        }
        else
        {
            e->destory();
        }
    }
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

    // Draw Score
    m_window.draw(m_text);

    // Render entities
    for (auto& e : m_entities.getEntities())
    {
        // Calculate the position of the entity
        float posX = e->cTransform->pos.x;
        float posY = e->cTransform->pos.y;

        // Set the position of the shape in the entity
        e->cShape->circle.setPosition(posX, posY);

        // Rotate the shape for visual interest
        float angle = 1.0f;
        e->cShape->circle.rotate(angle);

        // Draw the shape of the entity
        m_window.draw(e->cShape->circle);
    }

    m_window.display();
}

