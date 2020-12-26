#include "pch.h"
#include "Entity.h"
#include "Entity_Container.h"
#include "Entity_Room.h"
#include "ObservationManager.h"
#include "GameLoader.h"
#include "World.h"

#pragma region Serialization
void Entity::WriteToJson(PrettyWriter<StringBuffer>* writer)
{
	if (parent.second != nullptr) {
		parentEntityID = parent.second->uniqueEntityID;
	}
	parentEntityDir = (int)parent.first;
	writer->Key("parentEntityID");
	writer->Int(parentEntityID);
	writer->Key("parentEntityDir");
	writer->Int(parentEntityDir);
	writer->Key("worldActive");
	writer->Bool(worldActive);
	writer->Key("worldID");
	writer->Int(worldID);
	writer->Key("uniqueEntityID");
	writer->Int(uniqueEntityID);
	writer->Key("worldActive");
	writer->Bool(worldActive);
	writer->Key("names");
	writer->StartArray();
	for (int i = 0; i < names.size(); i++) {
		writer->String(names[i].c_str(), static_cast<SizeType>(names[i].length()));
	}
	writer->EndArray();
}

void Entity::ReadFromJson(Value& v)
{
	worldID = v["worldID"].GetInt();
	parentEntityID = v["parentEntityID"].GetInt();
	parentEntityDir = v["parentEntityDir"].GetInt();
	worldActive = v["worldActive"].GetBool();
	uniqueEntityID = v["uniqueEntityID"].GetInt();
	worldActive = v["worldActive"].GetBool();
	for (int i = 0; i < v["names"].Size(); i++) {
		names.push_back(v["names"][i].GetString());
	}
}

void Entity::WriteData(std::fstream* output)
{
	output->write((char*)&worldID, sizeof(int));
	output->write((char*)& uniqueEntityID, sizeof(int));
	output->write((char*)& worldActive, sizeof(bool));
	int nameSize = names.size();
	output->write((char*)&nameSize, sizeof(int));
	for (int i = 0; i < nameSize; i++) {
		WriteStringData(names[i], output);
	}
	WriteStringData(individualName, output);
	WriteStringData(lookInfo, output);
	output->write((char*)&size, sizeof(float));
	output->write((char*)&weight, sizeof(float));
	output->write((char*)&internalVolume, sizeof(float));
	output->write((char*)&visibleInsides, sizeof(bool));
	output->write((char*)&countable, sizeof(bool));
	output->write((char*)&attachedToParent, sizeof(bool));
	output->write((char*)&rotation, sizeof(int));
	output->write((char*)&facingDirection, sizeof(int));
	if (parent.second != nullptr) {
		output->write((char*)&(parent.second->uniqueEntityID), sizeof(int)); //Writing parent id
	}
	else{
		int nullID = -1;
		output->write((char*)&nullID, sizeof(int));
	}
	output->write((char*)&(parent.first), sizeof(int));
	output->write((char*)&playerAccessible, sizeof(bool));
	//TODO adjectives
	int adjectivesCount = adjectives.size();
	output->write((char*)&(adjectivesCount), sizeof(int));
	for (int i = 0; i < adjectivesCount; i++) {
		output->write((char*)&(adjectives[i].first), sizeof(int));
		int subadjectivesCount = adjectives[i].second.size();
		output->write((char*)&(subadjectivesCount), sizeof(int));
		for (int j = 0; j < subadjectivesCount; j++) {
			WriteStringData(adjectives[i].second[j], output);
		}
	}
}

void Entity::ReadData(std::fstream* input)
{
	input->read((char*)&worldID, sizeof(int));
	input->read((char*)& uniqueEntityID, sizeof(int));
	input->read((char*)& worldActive, sizeof(bool));
	int nameSize;
	input->read((char*)&nameSize, sizeof(int));
	for (int i = 0; i < nameSize; i++) {
		names.push_back(ReadStringData(input));
	}
	individualName = ReadStringData(input);
	lookInfo = ReadStringData(input);
	input->read((char*)&size, sizeof(float));
	input->read((char*)&weight, sizeof(float));
	input->read((char*)&internalVolume, sizeof(float));
	input->read((char*)&visibleInsides, sizeof(bool));
	input->read((char*)&countable, sizeof(bool));
	input->read((char*)&attachedToParent, sizeof(bool));
	input->read((char*)&rotation, sizeof(int));
	input->read((char*)&facingDirection, sizeof(int));
	input->read((char*)&parentEntityID, sizeof(int));
	input->read((char*)&parentEntityDir, sizeof(int));
	input->read((char*)&playerAccessible, sizeof(bool));

	//TODO adjectives
	int adjectivesCount;
	input->read((char*)&(adjectivesCount), sizeof(int));
	for (int i = 0; i < adjectivesCount; i++) {
		Position p;
		input->read((char*)&(p), sizeof(int));

		std::vector<std::string> subAdjs;
		int subadjectivesCount;;
		input->read((char*)&(subadjectivesCount), sizeof(int));
		for (int j = 0; j < subadjectivesCount; j++) {
			subAdjs.push_back(ReadStringData(input));
		}
		adjectives.push_back(std::make_pair(p, subAdjs));
	}
}

void Entity::WriteStringData(std::string s, std::fstream* output)
{
	size_t len = s.size();
	output->write((char*)&(len), sizeof(size_t));
	output->write(s.c_str(), len);
}

std::string Entity::ReadStringData(std::fstream* input)
{
	size_t namelen;
	input->read((char*)&namelen, sizeof(size_t));
	char* temp = new char[namelen + 1];
	input->read(temp, namelen);
	temp[namelen] = '\0';
	return temp;
}
#pragma endregion

void Entity::SetEntityData(int id, bool visibleInsides, float internalVolume, float size, float weight)
{
	this->worldID = GameLoader::Instance().loadedTiles[0];
	this->size = size;
	this->internalVolume = internalVolume;
	this->visibleInsides = visibleInsides;
	this->uniqueEntityID = id;
	this->weight = weight;
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
	ObservationManager::Instance().MakeObservation(new Observation_Status("facing "+ ObservationManager::Instance().RotationToString(r),this));
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
	facingDirection = r;
	ObservationManager::Instance().MakeObservation(new Observation_Status(ObservationManager::Instance().FacingDirectionToString(facingDirection), this));
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
		ObservationManager::Instance().MakeObservation(new Observation_Movement(this->parent.first,this->parent.second,this));
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

float Entity::GetInternalVoidSPace()
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
		if (parentCheck->serializationID == hash) {
			*foundEntity = parentCheck;
			return true;
		}
		if (parentCheck != nullptr) {
			parentCheck = parentCheck->parent.second;
		}
	} while (parentCheck != nullptr);
	return false;
}

int Entity::GetChildDepth()
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

std::vector<Entity*> Entity::GetVisibleEntities(bool getsurrounding, bool getParent, bool getSelf, bool seeInside)
{
	//TODO:Redo this
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