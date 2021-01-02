#pragma once
#include "Entity.h"
class Entity_GroundTile : public Entity
{
public:
	Entity_GroundTile() {
		this->serializationID = 8;
	};
	virtual ~Entity_GroundTile() {};
	Entity_GroundTile* Clone() override {
		return new Entity_GroundTile(*this);
	}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

//TODO make protected?
	std::pair<std::string, int> toNorth = std::make_pair("",-1);
	std::pair<std::string, int> toEast = std::make_pair("", -1);
	std::pair<std::string, int> toSouth = std::make_pair("", -1);
	std::pair<std::string, int> toWest = std::make_pair("", -1);
	int worldPos = 0;
};

