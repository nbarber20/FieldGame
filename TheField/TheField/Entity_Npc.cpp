#include "pch.h"
#include "Entity_Npc.h"
#include "GameLoader.h"

void Entity_Npc::NoticePickpocket(Entity* thief)
{
	target = thief;
	AttackTarget(true);
}
