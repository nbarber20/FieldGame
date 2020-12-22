#pragma once
#include "Entity.h"
class Entity_Fluid : public Entity
{
public:
	Entity_Fluid() {
		this->countable = false;
	};
	virtual ~Entity_Fluid() {};

	virtual Entity_Fluid* Clone() {
		return new Entity_Fluid(*this);
	}
	virtual int GetClassHash() override {
		return typeid(this).hash_code();
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

	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
		writer->Key("hydration");
		writer->Double(hydration);
		writer->Key("swallowable");
		writer->Bool(swallowable);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
		hydration = v["hydration"].GetInt();
		swallowable = v["writer"].GetString();
	}

	virtual void Tick() override;
	virtual void Rotate(Rotation r) override;

	Entity* SplitFluid(float fluidToRemove);
	void MixFluid(Entity* toMixWith);
	
	float hydration;
	bool swallowable = true;
};

