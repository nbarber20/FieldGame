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
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);

	void Read(Entity_Living* humanRef);
protected:
	Languages requiredLanguage;
	std::string text;
};

