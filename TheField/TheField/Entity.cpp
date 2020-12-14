#include "pch.h"
#include "Entity.h"
#include "Entity_Container.h"
#include "ObservationManager.h"
#include "World.h"

int Entity::RandomRange(int start, int end)
{
	return (start + (std::rand() % (end - start + 1)));
}

void Entity::Rotate(Rotation r)
{
	this->rotation = r;

	if (r != Upright) {
		std::vector<Entity*> ontop = GetInventory(On);
		for (auto object : ontop) {
			object->SetParent(parent.first, parent.second);
			object->Rotate(Tipped);
		}
	}

	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_Rotation;
	o.referenceEntity = this;
	ObservationManager::Instance().MakeObservation(o);
}

void Entity::FaceClockWise()
{
	FacingDirection currentRot = this->facingDirection;
	FacingDirection newRot = North;
	switch (currentRot) {
	case North:
		newRot = East;
		break;
	case East:
		newRot = South;
		break;
	case South:
		newRot = West;
		break;
	case West:
		newRot = North;
		break;
	}
	this->Face(newRot);
	return;
}

void Entity::Face(FacingDirection r)
{

	ObservationManager::Observation o = ObservationManager::Observation();
	o.sense = ObservationManager::SENSE_Look;
	o.type = ObservationManager::TYPE_FacingDirection;
	o.referenceEntity = this;
	ObservationManager::Instance().MakeObservation(o);

	facingDirection = r;
}

bool Entity::SetParent(Position pos, Entity* newParent)
{
	return SetParent(pos, newParent, 0, false,true);
}

bool Entity::SetParent(Position pos, Entity* newParent, int roomIndex, bool attach, bool logObeservation)
{
	if (newParent != nullptr) {
		if (pos == Inside) {
			if (this->size > newParent->internalVolume) {
				return false;
			}
		}
	}
	if (logObeservation) {
		ObservationManager::Observation o = ObservationManager::Observation();
		o.sense = ObservationManager::SENSE_Look;
		o.type = ObservationManager::TYPE_Movement;
		o.referenceEntity = this;
		o.lastState = ObservationManager::Instance().PositionToString(this->parent.first, individualName);
		ObservationManager::Instance().MakeObservation(o);
	}

	if (parent.second != nullptr) parent.second->RemoveChild(this);
	parent = std::make_pair(pos, newParent);
	if (newParent != nullptr) newParent->AddChild(pos, this, roomIndex);
	attachedToParent = attach;
	return true;
}

Entity* Entity::SplitEntity()
{
	Entity* newEntity = Clone();
	World::Instance().AddEntity(newEntity);
	newEntity->parent = std::make_pair(Inside, nullptr);
	return newEntity;
}

std::vector<Entity*> Entity::GetInventory()
{
	std::vector<Entity*> entities;
	for (int x = 0; x < children.size(); x++) {
		for (int y = 0; y < children[x].second.size(); y++) {
			entities.push_back(children[x].second[y]);
		}
	}
	return entities;
}

std::vector<Entity*> Entity::GetInventory(Position p)
{
	std::vector<Entity*> e;
	for (int x = 0; x < children.size(); x++) {
		if (children[x].first == p) {
			for (int y = 0; y < children[x].second.size(); y++) {
				e.push_back(children[x].second[y]);
			}
		}
	}
	return e;
}

float Entity::getInternalVoidSPace()
{
	int filledSize = this->internalVolume;
	std::vector<Entity*> in = GetInventory(Inside);
	for (auto object : in) {
		filledSize -= object->size;
	}
	if (filledSize < 0) {
		filledSize = 0;
	}
	return filledSize;
}

void Entity::DropAllChildren()
{
	for (int x = 0; x < children.size(); x++) {
		for (int y = 0; y < children[x].second.size(); y++) {
			children[x].second[y]->parent = this->parent;
		}
	}
}

void Entity::AddAdjective(Position pos, std::string s)
{
	for (auto adj : adjectives) {
		if (adj.first == pos) {
			adj.second.push_back(s);
			return;
		}
	}
	//We do not have anything in that position currently
	std::pair<Position, std::vector<std::string>> pair = std::pair<Position, std::vector<std::string>>(pos, { s });
	adjectives.push_back(pair);
}
void Entity::RemoveAdjective(std::string s)
{
	for (int x = 0; x < adjectives.size(); x++) {
		for (int y = 0; y < adjectives[x].second.size(); y++) {
			if (std::find(adjectives[x].second.begin(), adjectives[x].second.end(), s) != adjectives[x].second.end()) {
				adjectives[x].second.erase(std::remove(adjectives[x].second.begin(), adjectives[x].second.end(), s), adjectives[x].second.end());
				return;
			}
		}
	}
}

std::vector<std::string> Entity::GetAdjectives()
{
	std::vector<std::string> ad;
	for (int x = 0; x < adjectives.size(); x++) {
		for (int y = 0; y < adjectives[x].second.size(); y++) {
			ad.push_back(adjectives[x].second[y]);
		}
	}
	return ad;
}

std::vector<std::string> Entity::GetAdjectives(Position p)
{
	std::vector<std::string> ad;
	for (int x = 0; x < adjectives.size(); x++) {
		if (adjectives[x].first == p) {
			for (int y = 0; y < adjectives[x].second.size(); y++) {
				ad.push_back(adjectives[x].second[y]);
			}
		}
	}
	return ad;
}

std::vector<std::string> Entity::GetAdjectivesBlacklisted(std::vector<Position> p)
{
	std::vector<std::string> ad;
	for (int x = 0; x < adjectives.size(); x++) {
		if (std::find(p.begin(), p.end(), adjectives[x].first) == p.end()) {
			for (int y = 0; y < adjectives[x].second.size(); y++) {
				ad.push_back(adjectives[x].second[y]);
			}
		}
	}
	return ad;
}

std::string Entity::GetRandomAdjective(Position p)
{
	for (int x = 0; x < adjectives.size(); x++) {
		if (adjectives[x].first == p) {
			if (adjectives[x].second.size() == 0)return"";
			return adjectives[x].second[RandomRange(0, adjectives[x].second.size() - 1)];
		}
	}
	return "";
}

bool Entity::CheckforNameMatch(Entity* toCompare)
{
	for (int i = 0; i < names.size(); i++) {
		for (int j = 0; j < toCompare->names.size(); j++) {
			if (names[i] == toCompare->names[j])return true;
		}
	}
	return false;
}

bool Entity::IsChildOf(Entity* toCompare)
{
	Entity* parentCheck = this->parent.second;
	do {
		if (parentCheck == toCompare)return true;
		if (parentCheck != nullptr) {
			parentCheck = parentCheck->parent.second;
		}
	} while (parentCheck!= nullptr);
	return false;
}

void Entity::AddChild(Position pos, Entity* toAdd, int roomIndex)
{

	for (int x = 0; x < children.size(); x++) {
		if (children[x].first == pos) {
			children[x].second.push_back(toAdd);
			return;
		}
	}
	//We do not have anything in that position currently
	std::vector<Entity*> newVector ={ toAdd };
	children.push_back(std::make_pair(pos,newVector));
}

bool Entity::RemoveChild(Entity* toRemove)
{
	for (int x = 0; x < children.size(); x++) {
		for (int y = 0; y < children[x].second.size(); y++) {
			if (std::find(children[x].second.begin(), children[x].second.end(), toRemove) != children[x].second.end()) {
				children[x].second.erase(std::remove(children[x].second.begin(), children[x].second.end(), toRemove), children[x].second.end());
				return true;
			}
		}
	}
	return false;
}