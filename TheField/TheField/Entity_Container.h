#pragma once
#include "Entity.h"
#include "Entity_Constructed.h"
#include "Entity_Fluid.h"
#include <iostream>
#include <istream>
#include <string>
class Entity_Container : public Entity_Constructed
{
public:
	Entity_Container(bool visibleInsides, float internalVolume,float size) :Entity_Constructed(visibleInsides, internalVolume,size)
	{
	}; 
	virtual Entity_Container* Clone() {
		return new Entity_Container(*this);
	}
	virtual void Tick() override;
	virtual bool PourInto(Entity* target);
};



