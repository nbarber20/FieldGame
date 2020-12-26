#include "pch.h"
#include "Entity_Food.h"

#pragma region Serialization
void Entity_Food::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("nutritionalValue");
	writer->Double(nutritionalValue);
	writer->Key("spoiled");
	writer->Bool(spoiled);
}

void Entity_Food::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	nutritionalValue = v["nutritionalValue"].GetDouble();
	spoiled = v["spoiled"].GetBool();
}

void Entity_Food::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&nutritionalValue, sizeof(float));
	output->write((char*)&spoiled, sizeof(bool));
}

void Entity_Food::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&nutritionalValue, sizeof(float));
	input->read((char*)&spoiled, sizeof(bool));
}
#pragma endregion

bool Entity_Food::GetSpoiled()
{
	return spoiled;
}

float Entity_Food::GetNutritionalValue()
{
	return nutritionalValue;
}
