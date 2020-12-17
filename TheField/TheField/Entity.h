#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include "TextDisplay.h"
#include <functional>


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
	Entity(bool visibleInsides, float internalVolume, float size) {
		this->size = size;
		this->internalVolume = internalVolume;
		this->visibleInsides = visibleInsides;
	}
	virtual ~Entity() {

	}

	virtual Entity* Clone() {
		return new Entity(*this);
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

	std::vector <std::string> names;
	std::string individualName;
	std::string lookInfo;
	std::vector<std::pair<Position, std::vector<std::string>>> adjectives;
	float size = 0;
	float internalVolume = 0;
	bool visibleInsides = false;
	bool coutable = true;
	std::pair < Position, Entity*> parent;
	bool attachedToParent = false;
	std::vector<std::pair<Position, std::vector<Entity*>>> children;
	Rotation rotation = Upright;
	FacingDirection facingDirection = North;
protected:
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex);
	virtual bool RemoveChild(Entity* toRemove);
};

