#include "Entity_Doorway.h"

#pragma region Serialization
void Entity_Doorway::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("toTile");
	writer->Int(toTile);
	writer->Key("doorDirection");
	writer->Int(doorDirection);
	writer->Key("open");
	writer->Bool(open);
	writer->Key("doorInfo");
	writer->String(doorInfo.c_str());
}

void Entity_Doorway::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	toTile = v["toTile"].GetInt();
	doorDirection = (FacingDirection)v["doorDirection"].GetInt();
	open = v["doorDirection"].GetBool();
	doorInfo = v["doorInfo"].GetString();
}

void Entity_Doorway::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&toTile, sizeof(int));
	output->write((char*)&doorDirection, sizeof(int));
	output->write((char*)&open, sizeof(bool));
	WriteStringData(doorInfo, output);
}

void Entity_Doorway::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&toTile, sizeof(int));
	input->read((char*)&doorDirection, sizeof(int));
	input->read((char*)&open, sizeof(bool));
	doorInfo = ReadStringData(input);
}
#pragma endregion