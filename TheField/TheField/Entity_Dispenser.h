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
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	Entity* DispenseEntity();
	int GetDispenserType();
protected:
	int dispenserType;
	std::string prefabName;
};

