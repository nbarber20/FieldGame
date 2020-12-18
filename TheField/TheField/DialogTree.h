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

	void WriteData(std::fstream* output) {
		int TreeNodesSize = TreeNodes.size();
		output->write((char*)&TreeNodesSize, sizeof(int));
		for (int i = 0; i < TreeNodesSize; i++) {
			WriteStringData(TreeNodes[i].dialog, output);
			output->write((char*)&(TreeNodes[i].event), sizeof(int));
			int TreeNodeResponseSize = TreeNodes[i].responses.size();
			output->write((char*)&(TreeNodeResponseSize), sizeof(int));
			for (int j = 0; j < TreeNodeResponseSize; j++) {
				WriteStringData(TreeNodes[i].responses[j].first, output);
				output->write((char*)&(TreeNodes[i].responses[j].second), sizeof(int));
			}
		}
	};
	void ReadData(std::fstream* input) {
		int TreeNodesSize;
		input->read((char*)&TreeNodesSize, sizeof(int));
		for (int i = 0; i < TreeNodesSize; i++) {
			std::string d = ReadStringData(input);
			int evnt;
			input->read((char*)&evnt, sizeof(int));

			int TreeNodeResponseSize;
			input->read((char*)&(TreeNodeResponseSize), sizeof(int));
			std::vector<std::pair<std::string, int>> responses;

			for (int j = 0; j < TreeNodeResponseSize; j++) {
				std::string f = ReadStringData(input);;
				int s;
				input->read((char*)&s, sizeof(int));
				responses.push_back(std::make_pair(f, s));
			}
			DialogNode node;
			node.dialog = d;
			node.event = (DialogTreeEvent)evnt;
			node.responses = responses;
			TreeNodes.push_back(node);
		}
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

