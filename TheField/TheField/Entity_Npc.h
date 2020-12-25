#pragma once
#include "Entity_Living.h"
#include "DialogTree.h"
class Entity_Npc : public Entity_Living
{
public:
	Entity_Npc() {
		SerializationID = 12;
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	};
	virtual ~Entity_Npc() {
		delete dialogTree;
	}
	virtual void WriteData(std::fstream* output) {
		Entity_Living::WriteData(output);
		dialogTree->WriteData(output);

	};
	virtual void ReadData(std::fstream* input) {
		Entity_Living::ReadData(input);
		dialogTree = new DialogTree();
		dialogTree->ReadData(input);
		dialogTree->LivingSource = this;
	};
	void NoticePickpocket(Entity* thief);
	DialogTree* dialogTree;
};

