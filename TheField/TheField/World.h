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
	int GetUniqueID();
	void Tick();
	void Setup();
	Entity* GetEntityByID(int id);
	void SaveTile(std::string filename);
	void LoadTile(std::string filename);
	Entity_Player* playerEntity;
private:
	std::vector< Entity*> entities;
	int uniqueID = 0;
};

