#pragma once
#define ASSET_MANAGER_EXPORT __declspec(dllexport)
#define uint unsigned int

#include <d3d11.h>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fstream>
#include "DDSTextureLoader.h"
#include "RenderUtils.h"
#include "LevelFormat.h"
#include "CommonUtils.h"
#include "cereal\cereal.hpp"

using namespace std;
using namespace DirectX;

//Disable warning about std::Map dll-interface -> typedef std::map<int, AssetManager::_scanFunc> _scanFuncMap;
#pragma warning( disable: 4251 )

struct MeshHeader24
{
	int _nameLength, _numberOfVertices, _subMeshID, _numberPointLights, _numberSpotLights;
};

struct MeshHeader26
{
	int _numberOfVertices, _numberPointLights, _numberSpotLights;
};

struct MeshHeader29
{
	int _numberOfVertices, _numberPointLights, _numberSpotLights;
};

struct MatHeader 
{
	int _diffuseNameLength, _specularNameLength;
};

struct SkeletonHeader
{
	unsigned int _version, _framerate, _boneCount, _actionCount;
};

static bool GetFilenamesInDirectory(char* folder, char* extension, vector<string> &listToFill, bool appendFullPath = true)
{
	bool result = false;
	if (folder != nullptr && extension != nullptr)
	{
		string search_path = folder;
		search_path.append("*");
		search_path.append(extension);
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			result = true;
			do {
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					string path = fd.cFileName;
					if (appendFullPath)
					{
						listToFill.push_back(folder + path);
					}
					else
					{
						listToFill.push_back(path);
					}
				}
			} while (FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}
	return result;
}

//AssetManager keeps lists of all assets in the game(currently meshes, textures and skeletal animation), and loads them onto the GPU or memory when required.
//RenderObjects are the interface by which the assetmanager provides models to the renderer. They contain at least one mesh, two colors for diffuse and specular respectively, two handles to textures, a pointer to its skeleton and various help variables.
//Get buffers with:
//assetManager->GetRenderObject(MY_OBJECT_RENDEROBJECT_INDEX).vertexBuffer; Also on meshlevel lies the size of the buffer and any lights that may be present.
//Call assetManager->UnLoad(MY_OBJECT_RENDEROBJECT_INDEX, false) if you think the mesh probably won't be needed again soon
//Unless otherwise signed all comments are by Fredrik
class ASSET_MANAGER_EXPORT AssetManager
{
private:

	typedef Mesh* (AssetManager::*_scanFunc)();
	typedef std::map<int, AssetManager::_scanFunc> _scanFuncMap;

	_scanFuncMap _meshFormatVersion;
	int _animationFormatVersion = 10, _idCounter = 0;
	ifstream* _infile;
	ID3D11Device* _device;
	vector<string>* _levelFileNames;
	vector<Skeleton*>* _skeletons;

	vector<RenderObject*>* _renderObjects;

	vector<Texture*>* _textures;
	vector<Mesh*>* _meshes;

	bool LoadModel(const std::string& name, Mesh* mesh);
	void Flush();
	Mesh* ScanModel24();
	Mesh* ScanModel26();
	Mesh* ScanModel27();
	Mesh* ScanModel28();
	Mesh* ScanModel29();
	Mesh* ScanModel(const std::string& name);
	Texture* ScanTexture(const std::string& name);
	Mesh* GetModel(const std::string& name);
	Skeleton* LoadSkeleton(const std::string& name);
	ID3D11Buffer* CreateVertexBuffer(vector<WeightedVertex> *weightedVertices, vector<Vertex> *vertices, int skeleton);

	int _textureIdCounter = 0;
public:
	AssetManager(ID3D11Device* device);
	~AssetManager();
	RenderObject* GetRenderObject(int index);
	RenderObject* GetRenderObject(const std::string& meshName, const std::string& textureName);
	HRESULT ParseLevelHeader(Level::LevelHeader* outputLevelHead, const std::string& levelHeaderFilePath);
	HRESULT ParseLevelBinary(Level::LevelBinary* outputLevelBin, const std::string& levelBinaryFilePath);
	Texture* GetTexture(std::string name);
	void Clean();
};
