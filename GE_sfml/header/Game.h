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
	sf::Text			m_instrunction;				// text to display game play pause inst
	sf::Image			m_icon;						// to load the icon
	sf::Texture			m_texture;					// to load the background
	sf::Sprite			m_background;
	sf::SoundBuffer		m_bufferHit;				// to Load hit audio file
	sf::SoundBuffer		m_bufferShoot;				// to Load Shoot audio file
	sf::SoundBuffer		m_bufferDeath;				// to Load player death audio file
	sf::SoundBuffer		m_bufferHitSE;				// to Load player death audio file
	sf::SoundBuffer		m_bufferPowerUp;			// to Load player death audio file
	sf::Sound			m_hitAudio;
	sf::Sound			m_shootAudio;
	sf::Sound			m_playerDeath;
	sf::Sound			m_hitSEAudio;
	sf::Sound			m_powerUpAudio;
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	long int			m_score = 0;				// player score
	int					m_currentFrame = 0;			
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;			// whether we updated game logic
	bool				m_running = true;			// whether the game is running
	int					m_playerInvulnerableTime = 1000; // Invulnerable time after player death spawn in milliseconds (1.0s)
	bool				m_playerInvulnerable = false;
	std::chrono::steady_clock::time_point m_playerInvulnerableStartTime; // Track invulnerability start time
	std::chrono::steady_clock::time_point m_lastSpecialWeaponTime;
	
	ptr<Entity> m_player;

	void init(const std::string& config);			// initialized the GameState with config file path

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