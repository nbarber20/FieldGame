#include "pch.h"
#include "Entity_Npc.h"
#include "GameLoader.h"

#pragma region Serialization

void Entity_Npc::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity_Living::WriteToJson(writer);
	writer->Key("dialogTree");
	writer->String(dialogTree->treeName.c_str());
}

void Entity_Npc::ReadFromJson(Value& v)
{
	Entity_Living::ReadFromJson(v);
	GameLoader::Instance().LoadDialogTree(v["dialogTree"].GetString());
}

void Entity_Npc::WriteData(std::fstream* output)
{
	Entity_Living::WriteData(output); 
	bool hasDialog = dialogTree != nullptr;
	output->write((char*)&hasDialog, sizeof(bool));
	if (hasDialog) {
		WriteStringData(dialogTree->treeName,output);
	}
}

void Entity_Npc::ReadData(std::fstream* input)
{
	Entity_Living::ReadData(input);
	bool hasDialog;
	input->read((char*)&hasDialog, sizeof(bool));
	if (hasDialog) {
		dialogTree = GameLoader::Instance().LoadDialogTree(ReadStringData(input));
	}
}
#pragma endregion

void Entity_Npc::AddDialogTree(DialogTree* newTree)
{
	dialogTree = newTree;
	newTree->livingSource = this;
}

void Entity_Npc::NoticePickpocket(Entity* thief)
{
	target = thief;
	AttackTarget(true);
}

DialogTree* Entity_Npc::GetDialogTree()
{
	return dialogTree;
}
