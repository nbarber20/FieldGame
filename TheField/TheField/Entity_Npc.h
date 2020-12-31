#pragma once
#include "Entity_Living.h"
#include "DialogTree.h"
class Entity_Npc : public Entity_Living
{
public:
	Entity_Npc() {
		serializationID = 12;
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	};
	virtual ~Entity_Npc() {
		delete dialogTree;
	}
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void NoticePickpocket(Entity* thief);

	DialogTree* dialogTree;
};

