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
	Entity_Clip(int bulletsInClip, int clipSize, ClipType type){
		serializationID = 1;
		this->clipType = type;
		this->bulletsInClip = bulletsInClip;
		this->clipSize = clipSize;
	}
	virtual ~Entity_Clip() {}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	int FireBullet(int toRemove);
	ClipType GetClipType();
protected:
	ClipType clipType;
	int bulletsInClip;
	int clipSize;
};

