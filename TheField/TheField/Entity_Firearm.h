#pragma once
#include "Entity.h"
#include "Entity_Clip.h"
class Entity_Firearm : public Entity
{
public:
	Entity_Firearm(float size, Entity_Clip::ClipType type) : Entity(false, 0.0f, size) 
	{
		this->clipType = type;
	};
	virtual ~Entity_Firearm() {};
	void Fire(Entity* target);
	void Reload(Entity* clip);
	Entity_Clip::ClipType clipType;
	bool loaded = false;
};

