#pragma once
#include <string>
#include <vector>
#include "BehaviorTree.h"
#include "Entity_Living.h"
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
		std::string behaviorTree = "";
	};

	DialogTree() {};
	~DialogTree() {
		treeNodes.clear();
	};

	void WriteData(std::fstream* output) {
		int TreeNodesSize = treeNodes.size();
		output->write((char*)&TreeNodesSize, sizeof(int));
		for (int i = 0; i < TreeNodesSize; i++) {
			WriteStringData(treeNodes[i].dialog, output);
			output->write((char*)&(treeNodes[i].event), sizeof(int));
			int TreeNodeResponseSize = treeNodes[i].responses.size();
			output->write((char*)&(TreeNodeResponseSize), sizeof(int));
			for (int j = 0; j < TreeNodeResponseSize; j++) {
				WriteStringData(treeNodes[i].responses[j].first, output);
				output->write((char*)&(treeNodes[i].responses[j].second), sizeof(int));
			}

			if (!treeNodes[i].behaviorTree.empty()) {

				std::string name = treeNodes[i].behaviorTree;
				size_t len = name.size();
				output->write((char*)&(len), sizeof(size_t));
				output->write(name.c_str(), len);
			}
			else
			{
				std::string empty = "";
				size_t len = empty.size();
				output->write((char*)&(len), sizeof(size_t));
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

			size_t namelen;
			input->read((char*)&namelen, sizeof(size_t));
			char* temp = new char[namelen + 1];
			input->read(temp, namelen);
			temp[namelen] = '\0';
			if (namelen > 0) {
				node.behaviorTree = temp;
			}

			treeNodes.push_back(node);

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

	DialogTreeEvent Respond(int chosenResponse);

	std::string treeName;
	std::vector<DialogTree::DialogNode> treeNodes;
	Entity_Living* livingSource;
	int currentIndex = 0;
};

