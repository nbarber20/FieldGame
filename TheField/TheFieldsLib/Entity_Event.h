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
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;
	bool AttemptTrigger();
protected:
	std::string eventText;
	std::string eventImageFile;
};

