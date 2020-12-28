#include "pch.h"
#include "Entity_Container.h"
#include "Entity_Fluid.h"
#include "ObservationManager.h"

#pragma region Serialization

void Entity_Container::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	Entity_Constructed::WriteToJson(writer);
	writer->Key("permiability");
	writer->Double(permiability);
}

void Entity_Container::ReadFromJson(Value& v)
{
	Entity_Constructed::ReadFromJson(v);
	permiability = v["permiability"].GetDouble();
}

void Entity_Container::WriteData(std::fstream* output)
{
	Entity_Constructed::WriteData(output);
	output->write((char*)&permiability, sizeof(float));

}

void Entity_Container::ReadData(std::fstream* input)
{
	Entity_Constructed::ReadData(input);
	input->read((char*)&permiability, sizeof(float));
}
#pragma endregion

void Entity_Container::Tick()
{
	Entity::Tick();
	if ((this->visibleInsides && this->rotation != Upright) || this->broken == true) {
		std::vector<Entity*> inside = GetInventory(Inside);
		for(auto object : inside){
			Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
			if (fluidCheck) {
				fluidCheck->AddAdjective(Visual, "spilled");
			}
			object->SetParent(parent.first, parent.second);
		}
	}
	if (this->permiability > 0) {
		std::vector<Entity*> inside = GetInventory(Inside);
		for (auto object : inside) {
			Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
			if (fluidCheck) {
				ObservationManager::Instance().MakeObservation(new Observation_Direct("Some " + fluidCheck->names[0] + " drips out of the " + this->names[0], fluidCheck));
				Entity* drippingFluid = fluidCheck->SplitFluid(permiability);
				drippingFluid->AddAdjective(Visual, "spilled");
				drippingFluid->SetParent(parent.first, parent.second);				
				break;
			}
		}
	}
}

bool Entity_Container::PourInto(Entity* target)
{
	if (target == this) {
		return false;
	}
	Position targetPositin = On;
	if (target->internalVolume > 0) {
		targetPositin = Inside;
	}
	std::vector<Entity*> inside = GetInventory(Inside);
	for (auto object : inside) {		
		//If this is inside, ensure there is space or pour as much as possible
		if (targetPositin == Inside) {
			float targetVoidSpace = target->GetInternalVoidSPace();
			if (object->size > targetVoidSpace) {
				Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
				if (fluidCheck) {
					fluidCheck->RemoveAdjective("spilled");
					fluidCheck->SplitFluid(targetVoidSpace)->SetParent(targetPositin,target);
				}
				continue;
			}
		}	
		Entity_Fluid* fluidCheck = dynamic_cast<Entity_Fluid*>(object);
		if (fluidCheck) {
			fluidCheck->RemoveAdjective("spilled");
		}
		object->SetParent(targetPositin, target);
	}
	if (GetInternalVoidSPace() == this->internalVolume){
		ObservationManager::Instance().MakeObservation(new Observation_Direct("The " + names[0] + " is empty", this));
		return true;
	}
	return true;
}
