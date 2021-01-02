#include "pch.h"
#include "DialogTree.h"
#include "GameLoader.h"

DialogTree::DialogTreeEvent DialogTree::Respond(int chosenResponse)
{
	if (chosenResponse < 0 || chosenResponse >= treeNodes[currentIndex].responses.size()) {
		currentIndex = 0;
		return EVENT_EXIT;
	}
	currentIndex = treeNodes[currentIndex].responses[chosenResponse].second;
	if (!treeNodes[currentIndex].behaviorTree.empty()) {
		livingSource->AddBehavior(GameLoader::Instance().LoadBehaviorTree(treeNodes[currentIndex].behaviorTree));
	}
	if (treeNodes[currentIndex].event == EVENT_EXIT) {
		currentIndex = 0;
	}

	return treeNodes[currentIndex].event;
}

