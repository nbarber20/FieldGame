#pragma once
#include "Entity.h"
class Entity_Fluid : public Entity
{
public:
	Entity_Fluid(float hydration, bool swallowable = true) {
		serializationID = 6;
		this->countable = false;
		this->hydration = hydration;
		this->swallowable = swallowable;
	}
	virtual ~Entity_Fluid() {}

	virtual Entity_Fluid* Clone() {
		return new Entity_Fluid(*this);
	}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	virtual void Tick() override;
	virtual void Rotate(Rotation r) override;
	Entity* SplitFluid(float fluidToRemove);
	void MixFluid(Entity* toMixWith);
	float GetHydration();
	bool GetSwallowable();
protected:
	float hydration;
	bool swallowable;
};

