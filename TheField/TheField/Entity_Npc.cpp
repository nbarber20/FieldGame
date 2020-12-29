#include "pch.h"
#include "Entity_Npc.h"
#include "GameLoader.h"

#pragma region Serialization
void Entity_Npc::WriteData(std::fstream* output)
{
	Entity_Living::WriteData(output); 
	bool hasDialog = dialogTree != nullptr;
	output->write((char*)&hasDialog, sizeof(bool));
	if (hasDialog) {
		dialogTree->WriteData(output);
	}

}

void Entity_Npc::ReadData(std::fstream* input)
{
	Entity_Living::ReadData(input);
	bool hasDialog;
	input->read((char*)&hasDialog, sizeof(bool));
	if (hasDialog) {
		dialogTree = new DialogTree();
		dialogTree->ReadData(input);
		dialogTree->LivingSource = this;
	}
}
#pragma endregion

void Entity_Npc::NoticePickpocket(Entity* thief)
{
	target = thief;
	AttackTarget(true);
}
