#pragma once
#include "Entity_Constructed.h"
class Entity_Container : public Entity_Constructed
{
public:
	Entity_Container(float permiability, int resisitance, bool broken = false) : Entity_Constructed(resisitance,broken){
		this->serializationID = 3;
		this->permiability = permiability;
	}
	virtual ~Entity_Container() {}
	Entity_Container* Clone() override {
		return new Entity_Container(*this);
	}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	virtual void Tick() override;
	virtual bool PourInto(Entity* target);
protected:
	float permiability = 0.0f;
};



