#pragma once
#include "Entity.h"
class Entity_GroundTile : public Entity
{
public:
	Entity_GroundTile() {
		this->serializationID = 8;
	};
	virtual ~Entity_GroundTile() {};
	virtual Entity_GroundTile* Clone() {
		return new Entity_GroundTile(*this);
	}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

//TODO make protected?
	std::pair<std::string, int> toNorth = std::make_pair("",-1);
	std::pair<std::string, int> toEast = std::make_pair("", -1);
	std::pair<std::string, int> toSouth = std::make_pair("", -1);
	std::pair<std::string, int> toWest = std::make_pair("", -1);
	int worldPos = 0;
};

