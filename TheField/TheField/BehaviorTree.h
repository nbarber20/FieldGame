#pragma once
#include <vector>
#include <string>
#include "Entity.h"
class BehaviorNode;
class BehaviorTree
{
public:
	BehaviorTree(std::string name, bool looping) {
		this->treeName = name;
		this->loop = looping;
	};
	~BehaviorTree() {
		for (int i = 0; i < nodes.size(); i++) {
			delete nodes[i];
		}
		nodes.clear();
	};
	bool Tick();
	void SetWaitReturn(BehaviorNode* node);
	void AddNode(BehaviorNode* newNode, BehaviorNode* parentNode);
	std::vector<BehaviorNode*> nodes;
	int waitReturnIndex = -1;
	bool tickedOnce = false;
	bool waiting = false;
	bool loop = true;
	std::string treeName;
	Entity* parentEntity;
};

