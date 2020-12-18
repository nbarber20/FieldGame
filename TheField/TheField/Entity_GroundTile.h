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

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
	};
};

