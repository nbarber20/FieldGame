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
	Entity* GetEntityByID(int id, int worldID);
	void MoveToTile(int tileName);
	void SaveAll();
	void LoadAll();
	void SavePlayer();
	void SaveTile(std::string filename, int worldID);
	void LoadFile(std::string filename, bool loadCurrentTile);
	Entity_Player* playerEntity;
	int currentTile = 0;
private:
	Entity* currentGroundTile;
	std::vector< Entity*> entities;
	int uniqueID = 0;
};

