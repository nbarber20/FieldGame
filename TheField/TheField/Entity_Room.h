#pragma once
#include "Entity.h"
class Entity_Room : public Entity
{
public:
	Entity_Room() {
		serializationID = 15;
		this->visibleInsides = false;
	};
	virtual ~Entity_Room() {};
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;
};

