#include "pch.h"
#include "TreeLoader.h"
#include <iostream>
#include <cstdio>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "DialogTree.h"
using namespace rapidjson;

#pragma warning(disable : 4996)

void TreeLoader::ConvertBehaviorTree(std::string filepathInput)
{

}

DialogTree* TreeLoader::ConvertDialogTree(std::string filepathInput)
{
	FILE* fp = fopen(filepathInput.c_str(), "rb"); // non-Windows use "r"

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	DialogTree* newTree = new DialogTree();
	Document document;
	if (document.ParseStream(is).HasParseError() == false) {
		newTree->treeName = document["name"].GetString();
		GenericArray nodes = document["passages"].GetArray();
		for (const auto& node : nodes) {
			DialogTree::DialogNode newNode;
			std::string dialog = node["text"].GetString();
			dialog = dialog.substr(0, dialog.find_first_of('\n'));
			newNode.dialog = dialog;

			GenericArray responses = node["links"].GetArray();
			for (const auto& response : responses) {
				newNode.responses.push_back(std::make_pair(response["name"].GetString(), response["pid"].GetInt() - 1));
			}
			newTree->treeNodes.push_back(newNode);
		}
	}
	return newTree;

}

