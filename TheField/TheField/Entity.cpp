#include "pch.h"
#include "Entity.h"
#include "Entity_Container.h"
#include "Entity_Room.h"
#include "ObservationManager.h"
#include "GameLoader.h"
#include "World.h"

void Entity::SetEntityData(int id, bool visibleInsides, float internalVolume, float size, float weight)
{
	this->worldID = GameLoader::Instance().loadedTiles[0];
	this->size = size;
	this->internalVolume = internalVolume;
	this->visibleInsides = visibleInsides;
	this->uniqueEntityID = id;
	this->weight = weight;
}

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

bool Entity::SetParentOverride(Position pos, Entity* newParent)
{
	if (parent.second != nullptr) parent.second->RemoveChild(this);
	parent = std::make_pair(pos, newParent);
	if (newParent != nullptr) newParent->AddChild(pos, this, 0);
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
	int attempts = 0;
	do {
		if (parentCheck == toCompare)return true;
		if (parentCheck != nullptr) {
			parentCheck = parentCheck->parent.second;
		}
		attempts++;
	} while (parentCheck!= nullptr&& attempts<10);
	return false;
}

bool Entity::IsChildOf(int hash,Entity** foundEntity)
{
	Entity* parentCheck = this->parent.second;
	do {
		if (parentCheck->SerializationID == hash) {
			*foundEntity = parentCheck;
			return true;
		}
		if (parentCheck != nullptr) {
			parentCheck = parentCheck->parent.second;
		}
	} while (parentCheck != nullptr);
	return false;
}

int Entity::getChildDepth()
{
	int depth = 0;
	Entity* parentCheck = this->parent.second;
	do {
		if (parentCheck != nullptr) {
			parentCheck = parentCheck->parent.second;
			depth++;
		}
	} while (parentCheck != nullptr&& depth<10);
	return depth;
}

std::vector<Entity*> Entity::getVisibleEntities(bool getsurrounding, bool getParent, bool getSelf, bool seeInside)
{
	std::vector<Entity*> visible;

	if (getSelf) {
		std::vector<Entity*> onPerson = GetInventory();
		for (int i = 0; i < onPerson.size(); i++) {
			if (onPerson[i] != this) {
				visible.push_back(onPerson[i]);
				std::vector<Entity*> subEntity = onPerson[i]->GetInventory();
				for (int j = 0; j < subEntity.size(); j++) {
					if (seeInside == false && onPerson[i]->visibleInsides == false && subEntity[j]->parent.first == Inside) {
						continue;
					}
					visible.push_back(subEntity[j]);
					std::vector<Entity*> subEntity2 = subEntity[j]->GetInventory();
					for (int k = 0; k < subEntity2.size(); k++) {
						if (seeInside == false && subEntity[j]->visibleInsides == false && subEntity2[k]->parent.first == Inside) {
							continue;
						}
						visible.push_back(subEntity2[k]);
					}
				}
			}
		}
	}
	if (getsurrounding&& this->parent.second!=nullptr) {
		std::vector<Entity*> withintile = (parent.second)->GetInventory();
		for (int i = 0; i < withintile.size(); i++) {
			if (withintile[i] != this) {
				visible.push_back(withintile[i]);
				std::vector<Entity*> subEntity = withintile[i]->GetInventory();
				for (int j = 0; j < subEntity.size(); j++) {
					if (seeInside == false && withintile[i]->visibleInsides == false && (subEntity[j]->parent.first == Inside || subEntity[j]->parent.first == OnFloor)) {
						continue;
					}
					visible.push_back(subEntity[j]);
					std::vector<Entity*> subEntity2 = subEntity[j]->GetInventory();
					for (int k = 0; k < subEntity2.size(); k++) {
						if (seeInside == false && subEntity[j]->visibleInsides == false && (subEntity2[k]->parent.first == Inside || subEntity[j]->parent.first == OnFloor)) {
							continue;
						}
						visible.push_back(subEntity2[k]);
					}
				}
			}
		}
	}
	if (getParent) {
		Entity_Room* parentRoomTest = dynamic_cast<Entity_Room*>(parent.second);
		if (parentRoomTest) {
			visible.push_back(parentRoomTest);
			visible.push_back(parentRoomTest->parent.second);
		}
	}

	return visible;
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