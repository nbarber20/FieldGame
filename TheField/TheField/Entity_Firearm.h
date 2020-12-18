#pragma once
#include "Entity.h"
#include "Entity_Clip.h"
class Entity_Firearm : public Entity
{
public:
	Entity_Firearm() {
		typeID = "Entity_Firearm";
	};
	Entity_Firearm(int id, float size, Entity_Clip::ClipType type) : Entity(id, false, 0.0f, size)
	{
		typeID = "Entity_Firearm";
		this->clipType = type;
	};
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&clipType, sizeof(int));
		output->write((char*)&loaded, sizeof(bool));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&clipType, sizeof(int));
		input->read((char*)&loaded, sizeof(bool));
	};

	virtual ~Entity_Firearm() {};
	void Fire(Entity* target);
	void Reload(Entity* clip);
	Entity_Clip::ClipType clipType;
	bool loaded = false;
};

