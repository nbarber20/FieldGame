#include "pch.h"
#include "Entity_Fluid.h"
#include "World.h"

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