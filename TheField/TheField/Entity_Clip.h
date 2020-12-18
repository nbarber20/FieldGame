#pragma once
#include "Entity.h"
class Entity_Clip : public Entity
{
public:
	enum ClipType
	{
		Pistol,
	};
	Entity_Clip() {
		typeID = "Entity_Clip";
	};
	Entity_Clip(int id, float size, int clipSize, ClipType type) : Entity(id, false, 0.0f, size) {
		typeID = "Entity_Clip";
		this->clipSize = clipSize;
		this->bulletsInClip = clipSize;
		this->clipType = type;
	}
	virtual ~Entity_Clip() {};
	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&clipType, sizeof(int));
		output->write((char*)&bulletsInClip, sizeof(int));
		output->write((char*)&clipSize, sizeof(int));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&clipType, sizeof(int));
		input->read((char*)&bulletsInClip, sizeof(int));
		input->read((char*)&clipSize, sizeof(int));
	};

	int FireBullet(int toRemove) {
		if(toRemove>bulletsInClip){
			int in = bulletsInClip;
			bulletsInClip = 0;
			return in;
		}
		else {
			bulletsInClip-=toRemove;
			return toRemove;
		}
	}

	ClipType clipType;
	int bulletsInClip = 7;
	int clipSize = 7;
};

