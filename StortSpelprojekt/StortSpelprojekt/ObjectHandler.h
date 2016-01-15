#pragma once
#include <vector>
#include "GameObject.h"
#include "Unit.h"
#include "Trap.h"
#include "Architecture.h"
#include "Trigger.h"
#include "Tilemap.h"
#include "AssetManager.h"

/*
ObjectHandler
Used for updating gamelogic and rendering.

Stores pointers of all GameObjects in a std::vector

Handles GameObjects

Can add and remove GameObject pointers from the std::vector
Finds objects based on their objectID or vector index.
Finds an object and return its vector index
Returns all objects of a certain Type (i.e. Traps) as a seperate objectHandler
*/


//A struct containing 1 renderobject and all the gameobjects' matrices that uses said renderobject
struct RenderList
{
	RenderObject* _renderObject;
	vector<XMMATRIX> _modelMatrices;
};

class ObjectHandler
{
private:
	vector<vector<GameObject*>> _gameObjects;
	//vector<RenderList> _renderLists;

	Tilemap* _tilemap;

	int _idCount = 0;
	int _objectCount = 0;

	AssetManager* _assetManager;

public:
	ObjectHandler(ID3D11Device* device, AssetManager* assetManager);
	~ObjectHandler();

	//Add a gameobject
	GameObject* Add(Type type, /*int renderObjectID,*/ XMFLOAT3 position, XMFLOAT3 rotation);
	
	bool Remove(int ID);
	bool Remove(Type type, int ID);

	GameObject* Find(int ID);
	GameObject* Find(Type type, int ID);
	GameObject* Find(Type type, short index);
	//Returns a vector containing all gameobjects with the same type
	vector<GameObject*> GetAll(Type type);
	//Returns a list of a renderobject and matrices for all objects using the renderobject
	RenderList GetAll(int renderObjectID);
	vector<vector<GameObject*>>* GetGameObjects();

	int GetObjectCount() const;

	Tilemap* GetTileMap() const;
	void SetTileMap(Tilemap* tilemap);

	bool LoadLevel(int lvlIndex);

	void InitPathfinding();

	//Update gamelogic of all objects
	void Update();
	//Relase all object resources
	void Release();
};

