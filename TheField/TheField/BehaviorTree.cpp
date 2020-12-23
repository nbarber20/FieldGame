#include "pch.h"
#include "BehaviorTree.h"
#include "BehaviorNode.h"


bool BehaviorTree::Tick()
{
	if (waitReturnIndex != -1) {
		waiting = false;
		int last = waitReturnIndex;
		nodes[waitReturnIndex]->Execute();
		if (waitReturnIndex == last) {
			waitReturnIndex = -1;
		}
	}
	else {
		if (loop==true) {
			waiting = false;
			nodes[0]->Execute();
		}
		else if (loop == false && tickedOnce == false) {
			waiting = false;
			nodes[0]->Execute();
		}
		else {
			return true; //Tree done
		}
	}
	tickedOnce = true;
	return false;
}

void BehaviorTree::SetWaitReturn(BehaviorNode* node)
{
	auto it = std::find(nodes.begin(), nodes.end(), node);
	if (it != nodes.end())
	{
		int index = it - nodes.begin();
		this->waitReturnIndex = index;
	}
	else {
		this->waitReturnIndex = -1;
	}
}

void BehaviorTree::AddNode(BehaviorNode* newNode, BehaviorNode* parentNode)
{
	newNode->treeParent = this;
	if (parentNode != nullptr) {
		newNode->nodeParent = parentNode;
		parentNode->AddSubNode(newNode);
	}
	nodes.push_back(newNode);
}
