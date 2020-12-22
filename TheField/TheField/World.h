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
	World() {};
	void ClearEntities();
	void AddEntity(Entity* e);
	void RemoveEntity(Entity* e);
	void Tick();
	Entity* GetEntityByID(int id, int worldID);
	void MoveToTile(int tileName);
	void setupParents();
	std::vector< Entity*> GetEntities();
	float GetWorldTime();
	int GetWorldDay();
	void SetWorldTime(float to);
	void SetWorldDay(int to);
	Entity_Player* playerEntity;
private:
	std::vector< Entity*> entities;
	float worldTime = 0;
	int day = 0;
	Constants constants;
	bool sunSet = false;
};

