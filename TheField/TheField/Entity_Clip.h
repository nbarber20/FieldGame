#pragma once
#include "Entity.h"
class Entity_Clip : public Entity
{
public:
	enum ClipType
	{
		Pistol,
		MachineGun,
	};
	Entity_Clip() {
	};
	Entity_Clip(int clipSize, ClipType type){
		this->clipSize = clipSize;
		this->bulletsInClip = clipSize;
		this->clipType = type;
	}
	virtual ~Entity_Clip() {};
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
	}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("clipType");
		writer->Int((int)clipType);
		writer->Key("bulletsInClip");
		writer->Int(bulletsInClip);
		writer->Key("clipSize");
		writer->Int(clipSize);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		clipType = (ClipType)v["clipType"].GetInt();
		bulletsInClip = v["bulletsInClip"].GetInt();
		clipSize = v["clipSize"].GetInt();
	}

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

