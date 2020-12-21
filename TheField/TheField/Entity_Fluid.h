#pragma once
#include "Entity.h"
class Entity_Fluid : public Entity
{
public:
	Entity_Fluid() {
		typeID = "Entity_Fluid";
		this->countable = false;
	};
	virtual ~Entity_Fluid() {};

	virtual Entity_Fluid* Clone() {
		return new Entity_Fluid(*this);
	}

	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);
		output->write((char*)&hydration, sizeof(float));
		output->write((char*)&swallowable, sizeof(bool));

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
		input->read((char*)&hydration, sizeof(float));
		input->read((char*)&swallowable, sizeof(bool));
	};
	

	virtual void Tick() override;
	virtual void Rotate(Rotation r) override;

	Entity* SplitFluid(float fluidToRemove);
	void MixFluid(Entity* toMixWith);
	
	float hydration;
	bool swallowable = true;
};

