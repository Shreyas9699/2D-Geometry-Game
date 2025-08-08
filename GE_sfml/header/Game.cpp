#include "Game.h"


Game::Game(const std::string& config)
{
	init(config);
    m_lastSpecialWeaponTime = std::chrono::steady_clock::now();
}

void Game::init(const std::string& config)
{
    // reads all necessary details about player, enemies, bullets from config files and loads it and configures the SFML window
    std::ifstream fin(config);

    // Check if the file is successfully opened 
    if (!fin.is_open()) 
    {
        std::cerr << "Error opening the file!" << "\n";
        exit;
    }

    // default values
    int winWidth = 1280, winHeight = 720, FPS = 60, fsMode = 1;

    std::string fontFileName;
    int fontSize = 20;
    int fontR = 255;
    int fontG = 255;
    int fontB = 255;

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
        m_window.create(sf::VideoMode(winWidth, winHeight), "Geometry War", sf::Style::Fullscreen);
    }
    else
    {
        m_window.create(sf::VideoMode(winWidth, winHeight), "Geometry War", sf::Style::Default, settings);
    }
    m_window.setFramerateLimit(FPS);

    if (!m_icon.loadFromFile("media/Icon.png")) {
        // Handle error if the image fails to load
        std::cerr << "Failed to the icon image, but still continuing\n";
    }
    else
    {
        m_window.setIcon(m_icon.getSize().x, m_icon.getSize().y, m_icon.getPixelsPtr());
    }
    
    if (!m_font.loadFromFile(fontFileName))
    {
        // if can't load the font file
        std::cerr << "Failed to load font\n";
        exit(-1);
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(fontSize);
    m_text.setFillColor(sf::Color(fontR, fontG, fontB));
    m_text.setPosition(0, 0);

    m_instrunction.setFont(m_font);
    m_instrunction.setCharacterSize(static_cast<int>(fontSize * 0.8));
    m_instrunction.setFillColor(sf::Color(fontR, fontG, fontB));
    m_instrunction.setString("Press P to Pause/Play ");
    sf::FloatRect textRect = m_instrunction.getLocalBounds();
    m_instrunction.setPosition(winWidth - textRect.width, 0);
    

    if (!m_texture.loadFromFile("media/BGIMG.png"))
    {
        std::cout << "Fail to upload the background Image\n";
    }
    m_background.setTexture(m_texture);
    float scale = std::min(static_cast<float>(winWidth) / m_texture.getSize().x, static_cast<float>(winHeight) / m_texture.getSize().y);
    m_background.setScale(scale, scale);

    // load the sound effects
    if (!m_bufferHit.loadFromFile("media/audio/explosion.wav"))
    {
        std::cout << "Failed to load audio file\n";
        exit(-1);
    }
    m_hitAudio.setBuffer(m_bufferHit);

    if (!m_bufferShoot.loadFromFile("media/audio/laserShoot.wav"))
    {
        std::cout << "Failed to load audio file\n";
        exit(-1);
    }
    m_shootAudio.setBuffer(m_bufferShoot);


    if (!m_bufferDeath.loadFromFile("media/audio/Hurt.wav"))
    {
        std::cout << "Failed to load audio file\n";
        exit(-1);
    }
    m_playerDeath.setBuffer(m_bufferDeath);

    if (!m_bufferHitSE.loadFromFile("media/audio/explosionSmall.wav"))
    {
        std::cout << "Failed to load audio file\n";
        exit(-1);
    }
    m_hitSEAudio.setBuffer(m_bufferHitSE);

    if (!m_bufferPowerUp.loadFromFile("media/audio/powerUp.wav"))
    {
        std::cout << "Failed to load audio file\n";
        exit(-1);
    }
    m_powerUpAudio.setBuffer(m_bufferPowerUp);

    // spawn player entity
    spawnPlayer();
}

void Game::run()
{
    while (m_running)
    {
        m_entities.update();
        sEnemySpawner();
        sUserInput();
        if (!m_paused)
        {
            sMovement();
            sCollision();
            sRender();
            sLifeSpan();
            m_currentFrame++;
        }
    }
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
    // Set player invulnerable and record start time
    m_playerInvulnerable = true;
    m_playerInvulnerableStartTime = std::chrono::steady_clock::now();
}

void Game::sEnemySpawner()
{
    // Users enemySpawnInterval to determine when the next emeny will be spawned
    if (m_currentFrame >= m_lastEnemySpawnTime + m_enemyConfig.SI)
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
    std::random_device rd;
    srand(rd()); // Seed with a more unpredictable value, to avoid deterministic behavior.
    float posX = m_enemyConfig.SR + (rand() % (m_window.getSize().x - 2 * m_enemyConfig.SR)) + 1;
    float posY = m_enemyConfig.SR + (rand() % (m_window.getSize().y - 2 * m_enemyConfig.SR)) + 1;

    // Random Vertices
    int randV = m_enemyConfig.VMIN + (rand() % (1 + m_enemyConfig.VMAX - m_enemyConfig.VMIN));

    // Randown Speed
    int randS = m_enemyConfig.SMIN + (rand() % (int)(1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN));

    // Randowm Colors
    int RGBMax = 255;
    int randR = (rand() % RGBMax);
    int randG = (rand() % RGBMax);
    int randB = (rand() % RGBMax);
    int randScore = (rand() % (500 - 100 + 1) / 5) * 5 + 100;

    e->cTransform = std::make_shared<CTransform>(Vec2(float(posX), float(posY)), Vec2(float(randS), float(randS)), 1.0f);
    e->cShape = std::make_shared<CShape>(m_enemyConfig.SR, randV, sf::Color(randR, randG, randB), sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB), m_playerConfig.OT);
    e->cLifeSpan = std::make_shared<CLifeSpan>(m_enemyConfig.L); // frames is the life span of an enemy
    e->cScore = std::make_shared<CScore>(randScore);
}

void Game::spawnSmallEnemy(ptr<Entity> e)
{
    /*  Spawn small enemies at the loc of the input enemy e
        When we create smaller enemy, we have to read the values of the original enemy
            - spawn a number of smaller enimies equal to the vertices of the original enemy
            - set eacg small enemy to the same color or org, half the size
            - small enimies are woth double points of the original enemy 
     */

    const int numVertices = e->cShape->circle.getPointCount();
    const int r = e->cShape->circle.getRadius();
    const float anglePerVertex = static_cast<float>((2 * M_PI) / numVertices);
    const sf::Color fillColor = e->cShape->circle.getFillColor();
    const sf::Color outlineColor = e->cShape->circle.getOutlineColor();
    const int OT = e->cShape->circle.getOutlineThickness();
    const int score = e->cScore->score * 2;

    for (int i = 0; i < numVertices; i++)
    {
        auto smallEnemy = m_entities.addEntity("SmallEnemy");
        
        float angle = anglePerVertex * (i + 1);
        Vec2 offSetPos = Vec2(std::cos(angle) * r, std::sin(angle) * r);
        
        Vec2 pos = e->cTransform->pos + offSetPos;
        Vec2 velocity = (e->cTransform->velocity + offSetPos) / (numVertices * 1.5);

        smallEnemy->cTransform = std::make_shared<CTransform>(pos, velocity, angle);
        smallEnemy->cShape = std::make_shared<CShape>(r / 2, numVertices, fillColor, outlineColor, OT);
        smallEnemy->cScore = std::make_shared<CScore>(score);
        smallEnemy->cLifeSpan = std::make_shared<CLifeSpan>(m_enemyConfig.L/ (numVertices * 5/2));
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
    Vec2 newPos = e->cTransform->pos + Vec2(m_playerConfig.SR * dir.x, m_playerConfig.SR * dir.y);
    bulletEntity->cTransform = std::make_shared<CTransform>(newPos, Vec2(m_bulletConfig.S, m_bulletConfig.S), angle);
    bulletEntity->cTransform->velocity = dir * m_bulletConfig.S;
    bulletEntity->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
    bulletEntity->cLifeSpan = std::make_shared<CLifeSpan>(m_bulletConfig.L);
}

void Game::spawnSpecialWeapon(ptr<Entity> e)
{
    /*Implement special weapon when right clicked
        get continuous bullets from each vertices of the player while player keeps moving, for like 10 seconds*/

    // Get the current time
    auto currentTime = std::chrono::steady_clock::now();

    std::thread bulletThread([this, e]() {
        const int numVertices = e->cShape->circle.getPointCount();
        const float anglePerVertex = static_cast<float>((2 * M_PI) / numVertices);
        const int r = e->cShape->circle.getRadius();

        // Timing parameters
        constexpr int durationSeconds = 3; // Duration in seconds

        // Start time
        auto startTime = std::chrono::steady_clock::now();

        // Loop until the duration has elapsed
        while (true)
        {
            // Check if the duration has elapsed
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
            if (elapsedTime >= durationSeconds)
                break; // Exit the loop if duration has elapsed

            // Spawn bullets from each vertex of the player
            for (int i = 0; i < numVertices; i++)
            {
                float angle = anglePerVertex * (i + 1);
                Vec2 offSetPos = Vec2(std::cos(angle) * r, std::sin(angle) * r);
                Vec2 pos = e->cTransform->pos + offSetPos;
                spawnBullet(e, pos);
            }

            // Introduce a small delay between bullet spawns
            std::this_thread::sleep_for(std::chrono::milliseconds(150)); // Adjust delay as needed
        }
        });

    // Detach the thread so it runs independently
    bulletThread.detach();
}

void Game::sMovement()
{
    // Check if invulnerability should end
    if (m_playerInvulnerable) 
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_playerInvulnerableStartTime).count();
        if (elapsed >= m_playerInvulnerableTime) 
        {
            m_playerInvulnerable = false;
        }
    }
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
        if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
        {
            m_running = false;
            m_window.close();
        }

        // Event when P key to pause and resume the game
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P)
        {
            m_paused = !m_paused;
        }

        // event when WASD keys are pressed
        if (!m_paused)
        {
            m_player->cInput->down = sf::Keyboard::isKeyPressed(sf::Keyboard::W);
            m_player->cInput->up = sf::Keyboard::isKeyPressed(sf::Keyboard::S);
            m_player->cInput->left = sf::Keyboard::isKeyPressed(sf::Keyboard::A);
            m_player->cInput->right = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    // Spawn a bullet at the mouse click position
                    Vec2 target(float(event.mouseButton.x), float(event.mouseButton.y));
                    m_shootAudio.play();
                    spawnBullet(m_player, target);
                }
                else if (event.mouseButton.button == sf::Mouse::Right) 
                {
                    auto currentTime = std::chrono::steady_clock::now();
                    if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastSpecialWeaponTime) >= std::chrono::seconds(15))
                    {
                        //m_laserShootAudio.play();
                        m_lastSpecialWeaponTime = currentTime;
                        m_powerUpAudio.play();
                        spawnSpecialWeapon(m_player);
                    }
                }
            }
        }

    }
}

void Game::sLifeSpan()
{
    /* This code calcuates the lispane of entites such as Bullet and Enemy
            and apply fade effect as they come close to death */

    sf::Uint8 fadeQty = 0.0000001;
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

    for (auto& e : m_entities.getEntities("SmallEnemy"))
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

    /* collision between entities, player and enemies collision between enemies and bullets,  
       collision radius not the shape radius
    */
    for (auto& e : m_entities.getEntities("Enemy"))
    {
        for (auto& b : m_entities.getEntities("Bullet"))
        {
            // get distance between both enemy and bullet
            Vec2 posBullet = b->cTransform->pos;
            Vec2 posEnemy = e->cTransform->pos;
            float distanceBetween = posBullet.dist(posEnemy);
            int CRsum = m_enemyConfig.CR + m_bulletConfig.CR;

            // detect collision
            if (distanceBetween < CRsum)
            {
                m_score += e->cScore->score;
                m_hitAudio.play();
                spawnSmallEnemy(e);
                e->destory();
                b->destory();
            }
        }

        // player and enemy collision
        Vec2 posPlayer = m_player->cTransform->pos;
        Vec2 posEnemy = e->cTransform->pos;
        float distanceBetween = posPlayer.dist(posEnemy);
        int CRsum = m_enemyConfig.CR + m_playerConfig.CR;

        // detect collision
        if (!m_playerInvulnerable && distanceBetween < CRsum)
        {
            m_playerDeath.play();
            m_player->destory();
            spawnPlayer();
        }
    }

    for (auto& se : m_entities.getEntities("SmallEnemy"))
    {
        for (auto& b : m_entities.getEntities("Bullet"))
        {
            // get distance between both enemy and bullet
            Vec2 posBullet = b->cTransform->pos;
            Vec2 posEnemy = se->cTransform->pos;
            float distanceBetween = posBullet.dist(posEnemy);
            int CRsum = (m_enemyConfig.CR / 2) + m_bulletConfig.CR;

            // detect collision
            if (distanceBetween < CRsum)
            {
                m_score += se->cScore->score;
                m_hitSEAudio.play();
                se->destory();
            }
        }
    }
    m_text.setString("Score: " + std::to_string(m_score));
}

void Game::sRender()
{
    m_window.clear();

    // Draw background and Score
    m_window.draw(m_background);
    m_window.draw(m_text);
    m_window.draw(m_instrunction);

    // Render entities
    for (auto& e : m_entities.getEntities())
    {
        // Calculate the position of the entity
        float posX = e->cTransform->pos.x;
        float posY = e->cTransform->pos.y;

        // Set the position of the shape in the entity
        e->cShape->circle.setPosition(posX, posY);

        // If this entity is the player, handle blinking
        if (e == m_player && m_playerInvulnerable) 
        {
            int blinkPeriod = 10;
            bool blinkOn = ((m_currentFrame / blinkPeriod) % 2) == 0;
            sf::Color fill = e->cShape->circle.getFillColor();
            sf::Color outline = e->cShape->circle.getOutlineColor();
            fill.a = blinkOn ? 128 : 255;
            outline.a = blinkOn ? 128 : 255;
            e->cShape->circle.setFillColor(fill);
            e->cShape->circle.setOutlineColor(outline);
        }
        else if (e == m_player) 
        {
            // Ensure player is fully opaque when not invulnerable
            sf::Color fill = e->cShape->circle.getFillColor();
            sf::Color outline = e->cShape->circle.getOutlineColor();
            fill.a = 255;
            outline.a = 255;
            e->cShape->circle.setFillColor(fill);
            e->cShape->circle.setOutlineColor(outline);
        }

        // Rotate the shape for visual interest
        float angle = 1.0f;
        e->cShape->circle.rotate(angle);

        // Draw the shape of the entity
        m_window.draw(e->cShape->circle);
    }
    m_window.display();
}