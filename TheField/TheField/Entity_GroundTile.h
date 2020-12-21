#pragma once
#include "Entity.h"
class Entity_GroundTile : public Entity
{
public:
	Entity_GroundTile() {
		typeID = "Entity_GroundTile";
	};
	virtual ~Entity_GroundTile() {};
	virtual Entity_GroundTile* Clone() {
		return new Entity_GroundTile(*this);
	}
	virtual void WriteData(std::fstream* output) {
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
	};
	virtual void ReadData(std::fstream* input) {
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
	};
	std::pair<std::string, int> toNorth = std::make_pair("",-1);
	std::pair<std::string, int> toEast = std::make_pair("", -1);
	std::pair<std::string, int> toSouth = std::make_pair("", -1);
	std::pair<std::string, int> toWest = std::make_pair("", -1);
	int worldPos = 0;
};

