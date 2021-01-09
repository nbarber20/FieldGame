#pragma once
#include "Entity.h"
class Entity_Doorway : public Entity
{
public:
	Entity_Doorway(int toTile,FacingDirection doorDirection) {
		this->toTile = toTile;
		this->doorDirection = doorDirection;
		this->serializationID = 18;
	}
	virtual ~Entity_Doorway() {}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	int toTile;
	FacingDirection doorDirection;
	bool open = true;
	std::string doorInfo = ""; //leading to ___
};

