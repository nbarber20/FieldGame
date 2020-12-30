#pragma once
#include <string>
#include "Entity.h"

class Observation {
public:
	Observation(Entity* referenceEntity, int depth = 0) {
		this->referenceEntity = referenceEntity;
		this->depth = depth;
	}
	virtual ~Observation() {}
	Entity* referenceEntity;
	int depth;
	bool displayed = false;
	int serializationID = 0;
};

class Observation_Image : public Observation{
public:
	Observation_Image(std::string imageFile, Entity* referenceEntity, int depth = 0):Observation(referenceEntity,depth) {
		this->imageFile = imageFile;
		this->serializationID = 1;
	}
	std::string imageFile;
};


class Observation_Direct : public Observation {
public:
	Observation_Direct(std::string text, Entity* referenceEntity, int depth = 0):Observation(referenceEntity, depth) {
		this->text = text;
		this->serializationID = 2;
	}
	std::string text;
};

class Observation_Look : public Observation {
public:
	Observation_Look(FacingDirection direction, Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->direction = direction;
		this->directional = true;
		this->serializationID = 3;
	}
	Observation_Look(Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->directional = false;
	}
	FacingDirection direction;
	bool directional;
};

class Observation_Movement : public Observation {
public:
	Observation_Movement(Position lastPosition, Entity* lastParent,  Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->lastPosition = lastPosition;
		this->lastParent = lastParent;
		this->serializationID = 4;
	}
	Position lastPosition;
	Entity* lastParent;
};

class Observation_Action : public Observation {
public:
	Observation_Action(std::string SecPersonverb, std::string ThirdPersonverb, Entity* target, Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->SecPersonverb = SecPersonverb;
		this->ThirdPersonverb = ThirdPersonverb;
		this->target = target;
		this->serializationID = 5;
	}
	std::string SecPersonverb;
	std::string ThirdPersonverb;
	Entity* target;
};

class Observation_Status : public Observation {
public:
	Observation_Status(std::string statusString, Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->statusString = statusString;
		this->serializationID = 6;
	}
	std::string statusString;
};


class Observation_Sound : public Observation {
public:
	Observation_Sound(std::string sound, Entity* referenceEntity, int depth = 0) :Observation(referenceEntity, depth) {
		this->sound = sound;
		this->serializationID = 6;
	}
	std::string sound;
};