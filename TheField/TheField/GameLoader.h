#pragma once
#include <string.h>
#include "Entity.h"

class GameLoader
{
public:
	static GameLoader& Instance()
	{
		static GameLoader INSTANCE;
		return INSTANCE;
	}
	GameLoader() {};
	void Setup();
	int GetUniqueID();
	void SaveAll();
	bool LoadAll(std::string filename);

	void UnloadTiles();
	void LoadTiles();
	void SavePlayer();
	void LoadPlayer(bool getLoadedTiles);
	void SaveTile(int tileID);
	void LoadTile(int tileID);
	bool CreateNewGameFile(std::string filename);
	bool DeleteGameFile(std::string filename);
	void CopyGameFile(std::string from, std::string to);
	void ThrowFileError(std::string error);
	Entity* GenEntity(int hash);

	std::vector<int> loadedTiles = { 0 };
	int currentPlayerTile;
	Entity* currentGroundTile;
	std::string currentFilename = "null";

private:

	int uniqueID = 0;
	int errorCount = 0;
};

