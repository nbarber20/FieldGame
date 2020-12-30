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

	Entity_Fluid* Clone() override {
		return new Entity_Fluid(*this);
	}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void Tick() override;
	void Rotate(Rotation r) override;
	Entity* SplitFluid(float fluidToRemove);
	void MixFluid(Entity* toMixWith);
	float GetHydration();
	bool GetSwallowable();
protected:
	float hydration;
	bool swallowable;
};

