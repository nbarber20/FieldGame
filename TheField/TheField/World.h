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
	void MoveToTile(std::string tileName);
	void SaveAll();
	void LoadAll();
	void SavePlayer();
	void SaveTile(std::string filename);
	void LoadFile(std::string filename, bool loadCurrentTile);
	void RedestributeUniqueIDs();
	Entity_Player* playerEntity;
private:
	Entity* currentGroundTile;
	std::string currentTile = "000";
	std::vector< Entity*> entities;
	int uniqueID = 0;
};

