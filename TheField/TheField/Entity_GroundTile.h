#pragma once
#include "Entity.h"
class Entity_GroundTile : public Entity
{
public:
	Entity_GroundTile() {
		typeID = "Entity_GroundTile";
	};
	Entity_GroundTile(int id, bool visibleInsides, float internalVolume, float size) :Entity(id, visibleInsides,internalVolume,size) {
		typeID = "Entity_GroundTile";
	};
	virtual ~Entity_GroundTile() {};
	virtual Entity_GroundTile* Clone() {
		return new Entity_GroundTile(*this);
	}
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		WriteStringData(worldPos, output);
		WriteStringData(toNorth.first, output);
		WriteStringData(toNorth.second, output);
		WriteStringData(toEast.first, output);
		WriteStringData(toEast.second, output);
		WriteStringData(toSouth.first, output);
		WriteStringData(toSouth.second, output);
		WriteStringData(toWest.first, output);
		WriteStringData(toWest.second, output);
	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		worldPos = ReadStringData(input);
		std::string f, s;
		f = ReadStringData(input);
		s = ReadStringData(input);
		toNorth = std::make_pair(f, s);
		f = ReadStringData(input);
		s = ReadStringData(input);
		toEast = std::make_pair(f, s);
		f = ReadStringData(input);
		s = ReadStringData(input);
		toSouth = std::make_pair(f, s);
		f = ReadStringData(input);
		s = ReadStringData(input);
		toWest = std::make_pair(f, s);
	};
	std::pair<std::string, std::string> toNorth;
	std::pair<std::string, std::string> toEast;
	std::pair<std::string, std::string> toSouth;
	std::pair<std::string, std::string> toWest;
	std::string worldPos = "000";
};

