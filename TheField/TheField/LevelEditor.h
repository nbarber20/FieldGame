#pragma once

class LevelEditor
{
public:
	LevelEditor() {};
	~LevelEditor() {};


	void LoadFromJSON();
	void LoadPlayerFromJSON();
	void SaveToJSON();
	void SavePlayerToJSON();
	void SaveToFile(bool savePlayer);

};

