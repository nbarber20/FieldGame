#include "pch.h"
#include "Entity_Mechanisim.h"

Entity_Mechanisim::~Entity_Mechanisim()
{
}

void Entity_Mechanisim::WriteData(std::fstream* output)
{
	Entity::WriteData(output);

};

void Entity_Mechanisim::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
};

void Entity_Mechanisim::Tick()
{
}

bool Entity_Mechanisim::AttemptBehavior(std::string input, Entity* target)
{
	return false;
}

