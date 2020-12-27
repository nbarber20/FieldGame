#include "pch.h"
#include "Entity_Dispenser.h"
#include "Gameloader.h"

#pragma region Serialization
void Entity_Dispenser::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("prefabName");
	writer->String(prefabName.c_str(), static_cast<SizeType>(prefabName.length()));
	writer->Key("dispenserType");
	writer->Int(dispenserType);
}

void Entity_Dispenser::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	prefabName = v["prefabName"].GetString();
	dispenserType = v["dispenserType"].GetInt();
}
void Entity_Dispenser::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	WriteStringData(prefabName, output);
	output->write((char*)&dispenserType, sizeof(int));
}

void Entity_Dispenser::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	prefabName = ReadStringData(input);
	input->read((char*)&dispenserType, sizeof(int));
}
#pragma endregion

Entity* Entity_Dispenser::DispenseEntity()
{
	return GameLoader::Instance().SpawnPrefab(prefabName, Inside, this);
}

int Entity_Dispenser::GetDispenserType()
{
	return dispenserType;
}
