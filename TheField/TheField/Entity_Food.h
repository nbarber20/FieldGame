#pragma once
#include "Entity.h"
class Entity_Food : public Entity
{
public:
	Entity_Food(float nutritionalValue, bool spoiled = false) {
		this->serializationID = 7;
		this->nutritionalValue = nutritionalValue;
		this->spoiled = spoiled;
	}
	virtual ~Entity_Food(){}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;
	
	bool GetSpoiled();
	float GetNutritionalValue();
protected:
	float nutritionalValue;
	bool spoiled;
};

