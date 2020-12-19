#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include "TextDisplay.h"
#include <functional>
#include <fstream>

enum Languages
{
	English,
	Observer,
};


enum Position
{  
	Nowhere,
	InFront,
	Inside,
	Behind,
	On,
	Below,

	//Interiors
	OnWall,
	OnFloor,
	OnCieling,

	//Human specific
	RightHand,
	LeftHand,
	Arms,
	Chest,
	Back,
	Legs,
	Feet,
	Head,
	Mouth,

	//Adjective Hooks
	Visual,
	Taste,
	Smell,
	NameExtension,
};
enum Rotation
{
	Upright,
	UpsideDown,
	Tipped,
};
enum FacingDirection
{
	North,
	East,
	South,
	West,
};

class ObservationManager;

class Entity
{
public:
	Entity() {};
	Entity(int id,bool visibleInsides, float internalVolume, float size) {
		this->size = size;
		this->internalVolume = internalVolume;
		this->visibleInsides = visibleInsides;
		this->uniqueEntityID = id;
	}
	virtual ~Entity() {

	}

	virtual Entity* Clone() {
		return new Entity(*this);
	}

	virtual void WriteData(std::fstream* output) {
		WriteStringData(typeID, output);
		output->write((char*)&worldID, sizeof(int));
		output->write((char*)&uniqueEntityID, sizeof(int));
		int nameSize = names.size();
		output->write((char*)&nameSize, sizeof(int));
		for (int i = 0; i < nameSize; i++) {
			WriteStringData(names[i], output);
		}
		WriteStringData(individualName, output);
		WriteStringData(lookInfo, output);
		output->write((char*)&size, sizeof(float));
		output->write((char*)&internalVolume, sizeof(float));
		output->write((char*)&visibleInsides, sizeof(bool));
		output->write((char*)&countable, sizeof(bool));
		output->write((char*)&attachedToParent, sizeof(bool));
		output->write((char*)&rotation, sizeof(int));
		output->write((char*)&facingDirection, sizeof(int));
		//TODO adjectives
		if (parent.second != nullptr) {
			output->write((char*)&(parent.second->uniqueEntityID), sizeof(int));
		}
		else
		{
			int nullID = -1;
			output->write((char*)&nullID, sizeof(int));
		}
		output->write((char*)&(parent.first), sizeof(int));
	};
	virtual void ReadData(std::fstream* input) {
		input->read((char*)&worldID, sizeof(int));
		input->read((char*)&uniqueEntityID, sizeof(int));
		int nameSize;
		input->read((char*)&nameSize, sizeof(int));
		for (int i = 0; i < nameSize; i++) {
			names.push_back(ReadStringData(input));
		}
		individualName =ReadStringData(input);
		lookInfo =ReadStringData(input);
		input->read((char*)&size, sizeof(float));
		input->read((char*)&internalVolume, sizeof(float));
		input->read((char*)&visibleInsides, sizeof(bool));
		input->read((char*)&countable, sizeof(bool));
		input->read((char*)&attachedToParent, sizeof(bool));
		input->read((char*)&rotation, sizeof(int));
		input->read((char*)&facingDirection, sizeof(int));
		input->read((char*)&parentEntityID, sizeof(int));
		input->read((char*)&parentEntityDir, sizeof(int));

		//TODO adjectives
	};

	void WriteStringData(std::string s, std::fstream* output) {
		size_t len = s.size();
		output->write((char*)&(len), sizeof(size_t));
		output->write(s.c_str(), len);
	}
	std::string ReadStringData(std::fstream* input) {

		size_t namelen;
		input->read((char*)&namelen, sizeof(size_t));
		char* temp = new char[namelen + 1];
		input->read(temp, namelen);
		temp[namelen] = '\0';
		return temp;
	}

	virtual void Tick() {}

	int RandomRange(int start, int end);
	virtual void Rotate(Rotation r);
	void FaceClockWise();
	void Face(FacingDirection r);
	virtual bool SetParent(Position pos, Entity* newParent);
	virtual bool SetParentOverride(Position pos, Entity* newParent);
	virtual bool SetParent(Position pos, Entity* newParent, int roomIndex, bool attach, bool logObeservation);
	virtual Entity* SplitEntity();
	std::vector<Entity*> GetInventory();
	std::vector<Entity*> GetInventory(Position p); 
	float getInternalVoidSPace();
	void DropAllChildren();
	void AddAdjective(Position pos, std::string s);
	void RemoveAdjective(std::string s);
	std::vector<std::string> GetAdjectives();
	std::vector<std::string> GetAdjectives(Position p);
	std::vector<std::string> GetAdjectivesBlacklisted(std::vector<Position> p);
	std::string GetRandomAdjective(Position p);
	bool CheckforNameMatch(Entity* toCompare);
	bool IsChildOf(Entity* toCompare);


	//UsedForLoading
	int parentEntityID = -1;
	int parentEntityDir = 0;


	int worldID = 0;
	int uniqueEntityID = 0;
	std::vector <std::string> names;
	std::string individualName;
	std::string lookInfo;
	std::vector<std::pair<Position, std::vector<std::string>>> adjectives;
	float size = 0;
	float internalVolume = 0;
	bool visibleInsides = false;
	bool countable = true;
	std::pair < Position, Entity*> parent;
	bool attachedToParent = false;
	std::vector<std::pair<Position, std::vector<Entity*>>> children;
	Rotation rotation = Upright;
	FacingDirection facingDirection = North;
protected:
	std::string typeID = "Entity";
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex);
	virtual bool RemoveChild(Entity* toRemove);
};

