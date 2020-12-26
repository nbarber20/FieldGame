#include "pch.h"
#include "Entity_Fluid.h"
#include "World.h"

#pragma region Serialization
void Entity_Fluid::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity::WriteToJson(writer);
	writer->Key("hydration");
	writer->Double(hydration);
	writer->Key("swallowable");
	writer->Bool(swallowable);
}

void Entity_Fluid::ReadFromJson(Value& v)
{
	Entity::ReadFromJson(v);
	hydration = v["hydration"].GetInt();
	swallowable = v["writer"].GetString();
}

void Entity_Fluid::WriteData(std::fstream* output)
{
	Entity::WriteData(output);
	output->write((char*)&hydration, sizeof(float));
	output->write((char*)&swallowable, sizeof(bool));
}

void Entity_Fluid::ReadData(std::fstream* input)
{
	Entity::ReadData(input);
	input->read((char*)&hydration, sizeof(float));
	input->read((char*)&swallowable, sizeof(bool));
}
#pragma endregion

void Entity_Fluid::Tick()
{
	if (parent.second != nullptr) {
		std::vector<Entity*> nearby = parent.second->GetInventory(parent.first);
		for (int i = 0; i < nearby.size(); i++) {
			if (nearby[i] != this) {
				Entity_Fluid* fluid = dynamic_cast<Entity_Fluid*>(nearby[i]);
				if (fluid) {
					if (fluid->names == this->names) {
						this->MixFluid(fluid);
					}
				}
			}
		}
	}
}

void Entity_Fluid::Rotate(Rotation r)
{
	this->rotation = Upright;
}

Entity* Entity_Fluid::SplitFluid(float fluidToRemove)
{
	if (fluidToRemove >= this->size) {
		return this;
	}
	else {
		Entity* e = SplitEntity();
		this->size -= fluidToRemove;
		e->size = fluidToRemove;
		return e;
	}
}

void Entity_Fluid::MixFluid(Entity* toMixWith)
{
	toMixWith->size += this->size;
	for (int i = 0; i < adjectives.size(); i++) {
		for (int j = 0; j < adjectives[i].second.size(); j++) {
			toMixWith->AddAdjective(adjectives[i].first, adjectives[i].second[j]);
		}
	}
	World::Instance().RemoveEntity(this);
}

float Entity_Fluid::GetHydration()
{
	return hydration;
}

bool Entity_Fluid::GetSwallowable()
{
	return swallowable;
}
