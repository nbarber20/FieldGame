#pragma once
#include <vector>
#include "Entity.h"
#include "Entity_Player.h"
class World
{
public:
	static World& Instance()
	{
		static World INSTANCE;
		return INSTANCE;
	}
	World();
	void AddEntity(Entity* e);
	void RemoveEntity(Entity* e);
	void Tick();
	void Setup();
	Entity_Player* playerEntity;
private:
	std::vector< Entity*> entities;
};

