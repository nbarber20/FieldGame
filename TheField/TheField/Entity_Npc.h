#pragma once
#include "Entity_Living.h"
#include "DialogTree.h"
class Entity_Npc : public Entity_Living
{
public:
	Entity_Npc() {
		typeID = "Entity_Npc";
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	};
	virtual ~Entity_Npc() {
		delete dialogTree;
	}
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		dialogTree->WriteData(output);

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		dialogTree = new DialogTree();
		dialogTree->ReadData(input);
	};
	DialogTree* dialogTree;
};

