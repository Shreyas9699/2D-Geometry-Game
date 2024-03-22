#pragma once


#include "CommonHF.h"
#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig 
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; 
	float S;
};

struct EnemyConfig  
{
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
	float SMIN, SMAX;
};

struct BulletConfig 
{
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; 
	float S;
};

class Game
{
	sf::RenderWindow	m_window;					// the window we will draw o
	EntityManager		m_entities;					// vector of entities to maintain
	sf::Font			m_font;						// the font we will use to draw
	sf::Text			m_text;						// the score text to be drawn to the screen
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;				// player score
	int					m_currentFrame = 0;			
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;			// whether we updated game logic
	bool				m_running = true;			// whether the game is running

	ptr<Entity> m_player;

	void init(const std::string& config);			// initialized the GameState with config file path
	void setPaused(bool paused);					// pause the game

	void sMovement();								// System: Entity position / movement update
	void sUserInput();								// System: User Input
	void sLifeSpan();								// System: Lifespan
	void sRender();									// System: Render / Drawing
	void sEnemySpawner();							// System: Spawn Enemies
	void sCollision();								// System: Collision

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemy(ptr<Entity> e);
	void spawnBullet(ptr<Entity> e, const Vec2& mousePos);
	void spawnSpecialWeapon(ptr<Entity> e);

public:
	Game(const std::string& config); 
	void run();
};