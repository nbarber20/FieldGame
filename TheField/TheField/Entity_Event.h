#pragma once
#include "Entity.h"
class Entity_Event : public Entity
{
public:
	Entity_Event(std::string EventText, std::string EventImageFile = "") {
		this->serializationID = 4;
		this->nonPlayerFacing = true;
		this->eventText = EventText;
		this->eventImageFile = EventImageFile;
	};
	virtual ~Entity_Event() {};
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	bool AttemptTrigger();
protected:
	std::string eventText;
	std::string eventImageFile;
};

