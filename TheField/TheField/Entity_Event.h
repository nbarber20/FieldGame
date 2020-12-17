#pragma once
#include "Entity.h"
class Entity_Event : public Entity
{
public:
	Entity_Event() : Entity(false, 0.0f, 0.0f){
		
	}
	virtual ~Entity_Event() {
		ObservationTypeSenseConsumptionList.clear();
	};

	void setObservationConsumptionList(std::vector<std::pair<int, int>> newList);
	bool AttemptTrigger();
	std::string EventImageFile;
	std::string EventText;
private:
	std::vector<std::pair<int,int>> ObservationTypeSenseConsumptionList;
};

