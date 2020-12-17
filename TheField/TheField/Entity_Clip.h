#pragma once
#include "Entity.h"
class Entity_Clip : public Entity
{
public:
	enum ClipType
	{
		Pistol,
	};
	Entity_Clip(float size,int clipSize, ClipType type) : Entity(false, 0.0f, size){
		this->clipSize = clipSize;
		this->bulletsInClip = clipSize;
		this->clipType = type;
	}
	virtual ~Entity_Clip() {};


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

