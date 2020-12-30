#include "pch.h"
#include "Entity_Room.h"

#pragma region Serialization

void Entity_Room::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	//TODO
}

void Entity_Room::ReadFromJson(Value& v)
{
	//TODO
}

void Entity_Room::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
}

void Entity_Room::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
}
#pragma endregion
