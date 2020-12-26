#pragma once
#include "Entity.h"
class Entity_Dispenser : public Entity
{
public:
	Entity_Dispenser(int dispenserType, std::string prefabName) {
		this->serializationID = 17;
		this->dispenserType = dispenserType;
		this->prefabName = prefabName;
	}
	virtual ~Entity_Dispenser() {}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	Entity* DispenseEntity();
	int GetDispenserType();
protected:
	int dispenserType;
	std::string prefabName;
};

