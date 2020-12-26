#pragma once
#include "Entity.h"
class Entity_Anomaly : public Entity
{
public:
	Entity_Anomaly() {
		serializationID = 16;
	}
	~Entity_Anomaly() {}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
};

