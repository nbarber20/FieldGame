#pragma once
#include <string.h>
#include "Entity.h"
#include "BehaviorTree.h"
class GameLoader
{
public:
	static GameLoader& Instance()
	{
		static GameLoader INSTANCE;
		return INSTANCE;
	}
	GameLoader() {};
	void BackToTop();
	void Setup();
	int GetUniqueID();
	void SaveAll();
	bool LoadAll(std::string filename);
	Entity* SpawnPrefab(std::string filename, Position p, Entity* parent, bool fullPath = false);
	void SavePrefab(Entity* e, std::string filename);
	void SavePrefabToPath(Entity* e, std::string path);
	void UnloadTiles();
	void LoadTiles();
	BehaviorTree* LoadBehaviorTree(std::string filename);
	void SaveBehaviorTree(BehaviorTree* tree);
	void SavePlayer();
	void LoadPlayer(bool getLoadedTiles);
	void SaveTile(int tileID);
	void SaveTile(std::string filename, int tileID);
	void LoadTile(int tileID);
	void LoadTile(std::string filename, std::string copyFile);
	bool CreateNewGameFile(std::string filename);
	bool DeleteGameFile(std::string filename);
	void CopyGameFile(std::string from, std::string to);
	void ThrowFileError(std::string error);
	Entity* GenEntity(int hash);
	BehaviorNode* GenBehaviorNode(int hash);
	std::string GetDirectory();
	std::vector<int> loadedTiles = { 0 };
	int currentPlayerTile;
	Entity* currentGroundTile;
	std::string currentFilename = "null";

	bool exitOnFailure = true;
private:
	int uniqueID = 0;
	int errorCount = 0;
};

