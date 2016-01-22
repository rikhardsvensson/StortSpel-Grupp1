#pragma once
#include "Node.h"
#include "AssetManager.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"

#include <fstream>
#include <locale>
#include <codecvt>
#include <string>
#include <sstream>

namespace GUI
{
	class UITree
	{
	private:
		Node* _root;
		AssetManager* _AM;
		Node::NodeInfo _info;
	private:
		Node* LoadGUITree(const std::string& name, rapidjson::Value::ConstMemberIterator start, rapidjson::Value::ConstMemberIterator end);
		void Release(Node* node);
	public:
		UITree(const std::string& filename, const std::string& statename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height);
		~UITree();

		GUI::Node* GetRootNode() const;
		void Resize(int width, int height);
		void Resize(Node* current);
	};
}
