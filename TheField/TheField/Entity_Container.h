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
	virtual Entity_Container* Clone() {
		return new Entity_Container(*this);
	}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	virtual void Tick() override;
	virtual bool PourInto(Entity* target);
protected:
	float permiability = 0.0f;
};



