#include "pch.h"
#include "Entity_GroundTile.h"

#pragma region Serialization
void Entity_GroundTile::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("toNorthF");
	writer->String(toNorth.first.c_str(), static_cast<SizeType>(toNorth.first.length()));
	writer->Key("toNorthS");
	writer->Int(toNorth.second);
	writer->Key("toEastF");
	writer->String(toEast.first.c_str(), static_cast<SizeType>(toEast.first.length()));
	writer->Key("toEastS");
	writer->Int(toEast.second);
	writer->Key("toSouthF");
	writer->String(toSouth.first.c_str(), static_cast<SizeType>(toSouth.first.length()));
	writer->Key("toSouthS");
	writer->Int(toSouth.second);
	writer->Key("toWestF");
	writer->String(toWest.first.c_str(), static_cast<SizeType>(toWest.first.length()));
	writer->Key("toWestS");
	writer->Int(toWest.second);
}

void Entity_GroundTile::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	std::string f;
	int s;
	f = v["toNorthF"].GetString();
	s = v["toNorthS"].GetInt();
	toNorth = make_pair(f, s);
	f = v["toEastF"].GetString();
	s = v["toEastS"].GetInt();
	toEast = make_pair(f, s);
	f = v["toSouthF"].GetString();
	s = v["toSouthS"].GetInt();
	toSouth = make_pair(f, s);
	f = v["toWestF"].GetString();
	s = v["toWestS"].GetInt();
	toWest = make_pair(f, s);
}

void Entity_GroundTile::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&worldPos, sizeof(int));
	WriteStringData(toNorth.first, output);
	output->write((char*)&(toNorth.second), sizeof(int));
	WriteStringData(toEast.first, output);
	output->write((char*)&(toEast.second), sizeof(int));
	WriteStringData(toSouth.first, output);
	output->write((char*)&(toSouth.second), sizeof(int));
	WriteStringData(toWest.first, output);
	output->write((char*)&(toWest.second), sizeof(int));
}

void Entity_GroundTile::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&worldPos, sizeof(int));
	std::string f;
	int s;
	f = ReadStringData(input);
	input->read((char*)&s, sizeof(int));
	toNorth = std::make_pair(f, s);
	f = ReadStringData(input);
	input->read((char*)&s, sizeof(int));
	toEast = std::make_pair(f, s);
	f = ReadStringData(input);
	input->read((char*)&s, sizeof(int));
	toSouth = std::make_pair(f, s);
	f = ReadStringData(input);
	input->read((char*)&s, sizeof(int));
	toWest = std::make_pair(f, s);
}
#pragma endregion
