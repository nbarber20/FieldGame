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
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override; 
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	bool BreakConstructed(int inputStrength);
protected:
	int resistance;
	bool broken;
};

