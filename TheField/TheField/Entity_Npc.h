#pragma once
#include "Entity_Living.h"
#include "DialogTree.h"
class Entity_Npc : public Entity_Living
{
public:
	Entity_Npc(bool visibleInsides, float internalVolume, float size) : Entity_Living(visibleInsides, internalVolume, size)
	{
		spokenLanguage.push_back(English);
		readingLanguage.push_back(English);
	}
	virtual ~Entity_Npc() {
		delete dialogTree;
	}
	DialogTree* dialogTree;
};

