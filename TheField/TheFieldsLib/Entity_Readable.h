#pragma once
#include "Entity_Constructed.h"

class Entity_Living;
class ObservationManager;
class Entity_Readable : public Entity_Constructed
{
public:
	Entity_Readable(Languages requiredLanguage, std::string text, int resisitance, bool broken = false) : Entity_Constructed(resisitance,broken) {
		this->serializationID = 14;
		this->requiredLanguage;
		this->text;
	};
	virtual ~Entity_Readable() {};
	void WriteToJson(PrettyWriter<StringBuffer>* writer) override;
	void ReadFromJson(Value& v) override;
	void WriteData(std::fstream* output) override;
	void ReadData(std::fstream* input) override;

	void Read(Entity_Living* humanRef);
protected:
	Languages requiredLanguage;
	std::string text;
};

