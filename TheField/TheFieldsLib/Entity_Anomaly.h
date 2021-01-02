#pragma once
#include "Entity.h"
class Entity_Anomaly : public Entity
{
public:
	Entity_Anomaly() {
		serializationID = 16;
	}
	~Entity_Anomaly() {}
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;
};

