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
	bool LoadAll(std::string filename);
	void SavePlayer();
	void LoadPlayer(bool getLoadedTiles);
	void SaveTile(int tileID);
	void LoadTile(int tileID);
	bool CreateNewFile(std::string filename);
	bool DeleteFile(std::string filename);
	void CopyFile(std::string from,std::string to);
	void ThrowFileError(std::string error);

	Entity* genEntity(std::string entityObjType);
	void setupParents();
	

	Entity_Player* playerEntity;
	std::vector<int> loadedTiles = {0};
	int currentPlayerTile;
private:
	float worldTime = 0;
	int day = 0;
	Entity* currentGroundTile;
	std::vector< Entity*> entities;
	int uniqueID = 0;
	Constants constants;
	bool sunSet = false;
	std::string currentFilename = "null";
	int errorCount = 0;
};

