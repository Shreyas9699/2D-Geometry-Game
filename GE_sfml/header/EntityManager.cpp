#include "EntityManager.h"

EntityManager::EntityManager() {} // default

void EntityManager::update()
{
	// to add entities from m_entitiesToAdd the both vector of all entities and entity Map
	// remove all dead entities from both vector of all entities and entity Map

	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	// to have empty vec for next set of entities for next frame
	m_entitiesToAdd.clear();
	removeDeadEntities(m_entities);

	// only works for C++17+
	for (auto& [key, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// Since we need to remove the entity from entityvec while iterating thought, we chances of going into iterator invalidation
	// but in below case, we use the STL function called `remove_if`, which doesn't actually physically remove elements from the container. 
	// Instead, it reorders the elements in the range so that the elements to be removed are placed at the end of the range.
	// So, form the return iterator till end, we can directl use erase, since all removeable entities are at the end.
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](const auto& e)
		{
			return !e->isActive();
		}), vec.end());
}

ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(e);
	return e;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string tag)
{
	if (m_entityMap.find(tag) != m_entityMap.end())
	{
		return m_entityMap.find(tag)->second;
	}
	else
	{
		// if the tag its not in the game it return an empty vector
		static EntityVec emptyEntityVec; // Empty vector
		return emptyEntityVec;
	}
}