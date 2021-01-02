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

	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void AddDialogTree(DialogTree* newTree);
	void NoticePickpocket(Entity* thief);
	DialogTree* GetDialogTree();
private:
	DialogTree* dialogTree;
};

