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
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	
	bool GetSpoiled();
	float GetNutritionalValue();
protected:
	float nutritionalValue;
	bool spoiled;
};

