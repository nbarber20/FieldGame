#pragma once
#include "Entity.h"
class Entity_Fluid : public Entity
{
public:
	Entity_Fluid(bool visibleInsides, float internalVolume, float size) : Entity(visibleInsides, internalVolume, size)
	{
		this->coutable = false;
	};
	virtual ~Entity_Fluid() {};

	virtual Entity_Fluid* Clone() {
		return new Entity_Fluid(*this);
	}
	virtual void Tick() override;
	virtual void Rotate(Rotation r) override;

	Entity* SplitFluid(float fluidToRemove);
	void MixFluid(Entity* toMixWith);
	
	float hydration;
	bool swallowable = true;
};

