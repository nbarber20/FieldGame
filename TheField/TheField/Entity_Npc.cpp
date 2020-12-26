#include "pch.h"
#include "Entity_Npc.h"
#include "GameLoader.h"

#pragma region Serialization
void Entity_Npc::WriteData(std::fstream* output)
{
	Entity_Living::WriteData(output);
	dialogTree->WriteData(output);

}

void Entity_Npc::ReadData(std::fstream* input)
{
	Entity_Living::ReadData(input);
	dialogTree = new DialogTree();
	dialogTree->ReadData(input);
	dialogTree->LivingSource = this;
}
#pragma endregion

void Entity_Npc::NoticePickpocket(Entity* thief)
{
	target = thief;
	AttackTarget(true);
}
