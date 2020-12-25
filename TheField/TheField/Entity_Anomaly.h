#pragma once
#include "Entity.h"
class Entity_Anomaly : public Entity
{
public:
	Entity_Anomaly() {
		SerializationID = 16;
	}
	~Entity_Anomaly() {}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer) {
		Entity::WriteToJson(writer);
	}

	virtual void ReadFromJson(Value& v) {
		Entity::ReadFromJson(v);
	}

	virtual void WriteData(std::fstream* output) {
		Entity::WriteData(output);

	};
	virtual void ReadData(std::fstream* input) {
		Entity::ReadData(input);
	};
};

