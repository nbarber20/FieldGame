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
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	void NoticePickpocket(Entity* thief);

	//TODO save dialogTree seperate like behavior trees
	DialogTree* dialogTree;
};

