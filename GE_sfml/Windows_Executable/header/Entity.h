#pragma once

// Any Object in the Game, e.g. Player, platform, tile, Bullet, Enemy etc
// This program handles creation, and desctruction of any entity at any point in the game.

#include "CommonHF.h"
#include "Components.h"


class Entity
{
	friend class EntityManager; // to allow only EntityManager to construct Entity

	size_t		m_id = 0;
	std::string m_tag		= "default";
	bool		m_active = true;

	Entity(const size_t id, const std::string& tag);

public:
	ptr<CTransform>	cTransform;
	ptr<CShape>		cShape;
	ptr<CCollision>	cBBox;
	ptr<CScore>		cScore;
	ptr<CLifeSpan>	cLifeSpan;
	ptr<CInput>		cInput;

	bool isActive() const;
	const std::string& tag() const;
	const size_t id() const;
	void destory();

};