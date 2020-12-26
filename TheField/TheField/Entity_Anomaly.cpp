#include "pch.h"
#include "Entity_Anomaly.h"

#pragma region Serialization
void Entity_Anomaly::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
}

void Entity_Anomaly::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
}

void Entity_Anomaly::WriteData(std::fstream* output)
{
	Entity::WriteData(output);

}

void Entity_Anomaly::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
}
#pragma endregion