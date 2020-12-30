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
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	int FireBullet(int toRemove);
	ClipType GetClipType();
protected:
	ClipType clipType;
	int bulletsInClip;
	int clipSize;
};

