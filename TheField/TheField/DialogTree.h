#pragma once
#include <string>
#include <vector>
class DialogTree
{
public:
	enum DialogTreeEvent {
		EVENT_NONE,
		EVENT_EXIT,
	};
	
	struct DialogNode {
		std::string dialog;
		DialogTreeEvent event = EVENT_NONE;
		std::vector<std::pair<std::string, int>> responses;
	};

	DialogTree() {};
	~DialogTree() {
		TreeNodes.clear();
	};

	std::vector<DialogNode> TreeNodes;

	int currentIndex = 0;

	DialogTreeEvent Respond(int chosenResponse) {
		if (chosenResponse < 0 || chosenResponse >= TreeNodes[currentIndex].responses.size()) {
			currentIndex = 0;
			return EVENT_EXIT;
		}
		currentIndex = TreeNodes[currentIndex].responses[chosenResponse].second;

		if (TreeNodes[currentIndex].event == EVENT_EXIT) {
			currentIndex = 0;
		}

		return TreeNodes[currentIndex].event;
	}
};

