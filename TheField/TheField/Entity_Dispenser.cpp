#include "pch.h"
#include "Entity_Dispenser.h"
#include "Gameloader.h"

Entity* Entity_Dispenser::DispenseEntity()
{
	return GameLoader::Instance().SpawnPrefab(prefabName, Inside, this);
}
