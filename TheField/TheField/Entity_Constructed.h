#pragma once
#include "Entity.h"

class Entity_Constructed : public Entity
{
public:
	Entity_Constructed(int resisitance, bool broken = false) {
		serializationID = 2;
		this->resistance = resisitance;
		this->broken = broken;
	}
	virtual ~Entity_Constructed() {};
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v); 
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	bool BreakConstructed(int inputStrength);
protected:
	int resistance;
	bool broken;
};

