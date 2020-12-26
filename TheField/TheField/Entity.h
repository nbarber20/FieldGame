#pragma once
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include "TextDisplay.h"
#include <functional>
#include <fstream>
#include "rapidjson/document.h"     
#include "rapidjson/prettywriter.h" 
#include "Constants.h"

using namespace rapidjson;
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
	Entity() {
		serializationID = 0;
	};
	virtual ~Entity() {}
	virtual Entity* Clone() {
		return new Entity(*this);
	}
	virtual void WriteToJson(PrettyWriter<StringBuffer>* writer);
	virtual void ReadFromJson(Value& v);
	virtual void WriteData(std::fstream* output);
	virtual void ReadData(std::fstream* input);
	void WriteStringData(std::string s, std::fstream* output);
	std::string ReadStringData(std::fstream* input);

	virtual void SetEntityData(int id, bool visibleInsides, float internalVolume, float size, float weight);
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
	float GetInternalVoidSPace();
	void DropAllChildren();
	void AddAdjective(Position pos, std::string s);
	void RemoveAdjective(std::string s);
	std::vector<std::string> GetAdjectives();
	std::vector<std::string> GetAdjectives(Position p);
	std::vector<std::string> GetAdjectivesBlacklisted(std::vector<Position> p);
	std::string GetRandomAdjective(Position p);
	bool CheckforNameMatch(Entity* toCompare);
	bool IsChildOf(Entity* toCompare);
	bool IsChildOf(int hash, Entity** foundEntity);
	int GetChildDepth();
	virtual std::vector<Entity*>  GetVisibleEntities(bool getsurrounding, bool getParent, bool getSelf, bool seeInside = false);


	//UsedForLoading
	int serializationID = 0;
	int parentEntityID = -1;
	int parentEntityDir = 0;
	bool worldActive = false;


	int worldID = 0;
	int uniqueEntityID = 0;
	std::vector <std::string> names;
	std::string individualName;
	std::string lookInfo;
	std::vector<std::pair<Position, std::vector<std::string>>> adjectives;
	float size = 0;
	float weight = 0;
	float internalVolume = 0;
	bool visibleInsides = false;
	bool countable = true;
	bool nonPlayerFacing = false;
	std::pair < Position, Entity*> parent;
	bool attachedToParent = false;
	std::vector<std::pair<Position, std::vector<Entity*>>> children;
	Rotation rotation = Upright;
	FacingDirection facingDirection = North;
	Entity* target = nullptr;
	Entity* subTarget = nullptr;
	bool playerAccessible = false;
protected:
	Constants constants;
	virtual void AddChild(Position pos, Entity* toAdd, int roomIndex);
	virtual bool RemoveChild(Entity* toRemove);
};

