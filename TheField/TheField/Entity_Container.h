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
	Entity_Container() {
		typeID = "Entity_Container";
	};
	Entity_Container(int id, bool visibleInsides, float internalVolume,float size) :Entity_Constructed(id, visibleInsides, internalVolume,size)
	{
		typeID = "Entity_Container";
	}; 
	virtual ~Entity_Container() {};
	virtual Entity_Container* Clone() {
		return new Entity_Container(*this);
	}

	virtual void WriteData(std::fstream* output) {
		Entity_Constructed::WriteData(output);
		output->write((char*)&permiability, sizeof(float));

	};
	virtual void ReadData(std::fstream* input) {
		Entity_Constructed::ReadData(input);
		input->read((char*)&permiability, sizeof(float));
	};

	virtual void Tick() override;
	virtual bool PourInto(Entity* target);
	float permiability = 0.0f;
};



