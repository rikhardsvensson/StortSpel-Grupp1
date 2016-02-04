#include "ObjectHandler.h"
#include "stdafx.h"

ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data)
{
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;
	_buildingGrid = new Grid(device, 1, 1, 1, XMFLOAT3(1.0f, 1.0f, 0.7f));
	_gameObjectInfo = data;
	_device = device;

	
	ActivateTileset("default2");
}

ObjectHandler::~ObjectHandler()
{
	Release();
	SAFE_DELETE(_buildingGrid);
}

void ObjectHandler::Release()
{
	ReleaseGameObjects();
	SAFE_DELETE( _tilemap);
}

void ObjectHandler::ActivateTileset(const string& name)
{
	ReleaseGameObjects();
	_assetManager->ActivateTileset(name);

	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		for (uint i = 0; i < _gameObjectInfo->_objects[a]->size(); i++)
		{
			_gameObjectInfo->_objects[a]->at(i)->_renderObject = _assetManager->GetRenderObjectByType((Type)a, i);
		}
	}
}



bool ObjectHandler::Add(Type type, int index, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	GameObject* object = nullptr;

	switch (type)
	{
	case FLOOR:
		object = MakeFloor(_gameObjectInfo->Floors(index), position, rotation);
		break;
	case WALL:
		object = MakeWall(_gameObjectInfo->Walls(index), position, rotation);
		break;
	case LOOT:
		object = MakeLoot(_gameObjectInfo->Loot(index), position, rotation);
		break;
	case SPAWN:
		object = MakeSpawn(_gameObjectInfo->Spawns(index), position, rotation);
		break;
	case ENEMY:
		object = MakeEnemy(_gameObjectInfo->Enemies(index), position, rotation);
		break;
	case GUARD:
		object = MakeGuard(_gameObjectInfo->Guards(index), position, rotation);
		break;
	case TRAP:
		object = MakeTrap(_gameObjectInfo->Traps(index), position, rotation);
		break;
	default:	
		break;
	}


	//Temporary check for traps. Could be more general if other objects are allowed to take up multiple tiles
	bool addedObject = false;
	if (object != nullptr)
	{
		addedObject = true;
	}
	if (type == TRAP && addedObject)
	{
		Trap* trap = static_cast<Trap*>(object);
		int i = 0;
		AI::Vec2D* arr = trap->GetTiles();
		for (int i = 0; i < trap->GetTileSize() && addedObject; i++)
		{
			if (!_tilemap->CanPlaceObject(arr[i]))
			{
				addedObject = false;
			}
		}
		if (addedObject)
		{
			for (int i = 0; i < trap->GetTileSize() && addedObject; i++)
			{
				_tilemap->AddObjectToTile(arr[i], object);
				_tilemap->GetObjectOnTile(arr[i], FLOOR)->AddColorOffset({2,0,0});
			}
		}

	}

	if (object != nullptr)
	{
		_idCount++;
		_gameObjects[type].push_back(object);
		for(auto i : object->GetRenderObject()->_mesh._spotLights)
		{
			//TODO: Add settings variables to this function below! Alex
			_spotlights[object] = new Renderer::Spotlight(_device, i, 256, 256, 0.1f, 1000.0f);
		}
		_objectCount++;
		return true;
	}

	return false;
}

bool ObjectHandler::Add(Type type, const std::string& name, const XMFLOAT3& position = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rotation = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	for (unsigned int i = 0; i < _gameObjectInfo->_objects[type]->size(); i++)
	{
		if (_gameObjectInfo->_objects[type]->at(i)->_name == name)
		{
			return Add(type, i, position, rotation);
		}
	}
	return false;
}

bool ObjectHandler::Remove(int ID)
{
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (uint j = 0; j < _gameObjects[i].size(); j++)
		{
			if (_gameObjects[i][j]->GetID() == ID)
			{
				// Release object resource
				_gameObjects[i][j]->Release();

				if (_spotlights.count(_gameObjects[i][j]))
				{
					delete _spotlights[_gameObjects[i][j]];
					_spotlights.erase(_gameObjects[i][j]);
				}

				delete _gameObjects[i][j];

				// Replace pointer with the last pointer int the vector
				_gameObjects[i][j] = _gameObjects[i].back();

				// Remove pointer value to avoid various problems
				_gameObjects[i].pop_back();

				_objectCount--;

				return true;
			}
		}
	}


	//for (vector<GameObject*> v : _gameObjects)
	//{
	//	for (int i = 0; i < v.size(); i++)
	//	{
	//		if (v[i]->GetID() == ID)
	//		{
	//			// Release object resource
	//			v[i]->Release();

	//			delete v[i];

	//			// Replace pointer with the last pointer int the vector
	//			v[i] = v.back();

	//			// Remove pointer value to avoid various problems
	//			v.pop_back();

	//			_objectCount--;

	//			return true;
	//		}
	//	}
	//}
	return false;
}

bool ObjectHandler::Remove(Type type, int ID)
{
	for (uint i = 0; i < _gameObjects[type].size(); i++)
	{
		if (_gameObjects[type][i]->GetID() == ID)
		{
			_tilemap->RemoveObjectFromTile(_gameObjects[type].at(i));
			_gameObjects[type][i]->Release();

			if (_spotlights.count(_gameObjects[type][i]))
			{
				delete _spotlights[_gameObjects[type][i]];
				_spotlights.erase(_gameObjects[type][i]);
			}

			delete _gameObjects[type][i];

			// Replace pointer with the last pointer int the vector
			_gameObjects[type][i] = _gameObjects[type].back();

			// Remove pointer value to avoid various problems
			_gameObjects[type].pop_back();

			_objectCount--;


			return true;
		}
	}

	return false;
}

GameObject * ObjectHandler::Find(int ID)
{
	for (vector<GameObject*> v : _gameObjects)
	{
		for (GameObject* g : v)
		{
			if (g->GetID() == ID)
			{
				return g;
			}
		}
	}
	return nullptr;
}

bool ObjectHandler::Remove(GameObject* gameObject)
{
	return Remove(gameObject->GetType(), gameObject->GetID());
}

GameObject* ObjectHandler::Find(Type type, int ID)
{
	for (GameObject* g : _gameObjects[type])
	{
		if (g->GetID() == ID)
		{
			return g;
		}
	}
	return nullptr;
}

GameObject* ObjectHandler::Find(Type type, short index)
{
	for (GameObject* g : _gameObjects[type])
	{
		return g;
	}
	return nullptr;
}

vector<GameObject*> ObjectHandler::GetAllByType(Type type)
{
	return _gameObjects[type];
}

RenderList ObjectHandler::GetAllByType(int renderObjectID)
{
	// TODO make it so that it returns a vector of renderlists to improve performance /Markus

	RenderList list;
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);

	int count = 0;
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			if (g->GetRenderObject() == _assetManager->GetRenderObject(renderObjectID))
			{
				//TODO _modelMatrices should hold XMMATRIX* instead to ensure proper alignment of the matrices |Jonas a.k.a. ] 0 |\| /-\ 5
				list._modelMatrices[count] = *g->GetMatrix();
			}
			count++;
		}
	}

	return list;
}

vector<vector<GameObject*>>* ObjectHandler::GetGameObjects()
{
	return &_gameObjects;
}

GameObjectInfo * ObjectHandler::GetBlueprints()
{
	return _gameObjectInfo;
}

int ObjectHandler::GetObjectCount() const
{
	return _objectCount;
}


/*Tilemap handling*/

Tilemap * ObjectHandler::GetTileMap() const
{
	return _tilemap;
}

void ObjectHandler::SetTileMap(Tilemap * tilemap)
{
	delete _tilemap;
	_tilemap = tilemap;
}

void ObjectHandler::MinimizeTileMap()
{
	if (_tilemap)
	{
		int minY = -1, maxY = -1;
		for (int y = 0; y < _tilemap->GetHeight(); y++)
		{
			for (int x = 0; x < _tilemap->GetWidth() && minY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					minY = y;
				}
			}
		}
		for (int y = _tilemap->GetHeight() - 1; y >= minY; y--)
		{
			for (int x = 0; x < _tilemap->GetWidth() && maxY == -1; x++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxY = y + 1;
				}
			}
		}

		int minX = -1, maxX = -1;
		for (int x = 0; x < _tilemap->GetWidth(); x++)
		{
			for (int y = minY; y < maxY && minX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					minX = x;
				}
			}
		}

		for (int x = _tilemap->GetWidth() - 1; x >= minX; x--)
		{
			for (int y = minY; y < maxY && maxX == -1; y++)
			{
				if (!_tilemap->IsTileEmpty(x, y))
				{
					maxX = x + 1;
				}
			}
		}

		int newXMax = maxX - minX;
		int newYMax = maxY - minY;

		Tilemap* minimized = new Tilemap(AI::Vec2D(newXMax, newYMax));
		for (int x = 0; x < newXMax; x++)
		{
			for (int y = 0; y < newYMax; y++)
			{
				AI::Vec2D pos;
				pos._x = x + minX;
				pos._y = y + minY;

				std::vector<GameObject*> temp = _tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					// Update real pos
					g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

					// Update tile
					minimized->AddObjectToTile(x, y, g);
				}
			}
		}

		delete _tilemap;
		_tilemap = minimized;

		_buildingGrid->ChangeGridSize(_tilemap->GetWidth(), _tilemap->GetHeight(), 1);
	}
	else
	{
		throw std::runtime_error("ObjectHandler::MinimizeTileMap: _tilemap is nullptr");
	}
}

void ObjectHandler::EnlargeTilemap(int offset)
{
	if (offset > 0)
	{
		int o = 2 * offset;
		Tilemap* large = new Tilemap(AI::Vec2D(_tilemap->GetWidth() + o, _tilemap->GetHeight() + o));

		for (int x = offset; x < _tilemap->GetWidth() + offset; x++)
		{
			for (int y = offset; y < _tilemap->GetHeight() + offset; y++)
			{
				AI::Vec2D pos;
				pos._x = x - offset;
				pos._y = y - offset;
				std::vector<GameObject*> temp = _tilemap->GetAllObjectsOnTile(pos);

				for (GameObject* g : temp)
				{
					// Update real pos
					g->SetPosition(XMFLOAT3(x, g->GetPosition().y, y));

					// Update tile
					large->AddObjectToTile(x, y, g);
				}
			}
		}

		delete _tilemap;
		_tilemap = large;

		_buildingGrid->ChangeGridSize(large->GetWidth(), large->GetHeight(), 1);
	}
}

Grid * ObjectHandler::GetBuildingGrid()
{
	return _buildingGrid;
}




bool ObjectHandler::LoadLevel(int lvlIndex)
{
	int dimX, dimY;
	vector<GameObjectData> gameObjectData;
	bool result = _assetManager->ParseLevel(lvlIndex, gameObjectData, dimX, dimY);
	if (result)
	{
		if (_gameObjects.size() < 1)
		{
			for (int i = 0; i < NR_OF_TYPES; i++)
			{
				_gameObjects.push_back(std::vector<GameObject*>());
			}
		}

		delete _tilemap;
		_tilemap = new Tilemap(AI::Vec2D(dimX, dimY));

		for (auto i : gameObjectData)
		{
			Add((Type)i._tileType, 0, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
		}
	}
	return result;
}

void ObjectHandler::InitPathfinding()
{
	for (GameObject* i : _gameObjects[ENEMY])
	{
		Unit* unit = dynamic_cast<Unit*>(i);
		//unit->CheckAllTiles();
		unit->Move();
	}

	for (GameObject* i : _gameObjects[GUARD])
	{
		Unit* unit = dynamic_cast<Unit*>(i);
		//unit->CheckAllTiles();
		unit->Move();
	}
}

void ObjectHandler::EnableSpawnPoints()
{
	for (GameObject* g : _gameObjects[SPAWN])
	{
		((SpawnPoint*)g)->Enable();
	}
}

void ObjectHandler::DisableSpawnPoints()
{
	for (GameObject* g : _gameObjects[SPAWN])
	{
		((SpawnPoint*)g)->Disable();
	}
}

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (unsigned int j = 0; j < _gameObjects[i].size(); j++)
		{
			GameObject* g = _gameObjects[i][j];
			g->Update(deltaTime);

			if (g->GetPickUpState() == PICKINGUP)
			{
				_tilemap->RemoveObjectFromTile(g);
				g->SetPickUpState(HELD);
			}
			else if (g->GetPickUpState() == DROPPING)
			{
				_tilemap->AddObjectToTile(g->GetTilePosition(), g);
				g->SetPickUpState(ONTILE);
			}

			if (g->GetType() == GUARD || g->GetType() == ENEMY)									//Handle unit movement
			{
				Unit* unit = static_cast<Unit*>(g);

				GameObject* heldObject = unit->GetHeldObject();

				if (heldObject != nullptr)
				{
					heldObject->SetPosition(DirectX::XMFLOAT3(unit->GetPosition().x, unit->GetPosition().y + 2, unit->GetPosition().z));
				}

				if (unit->GetHealth() <= 0)
				{
					for (unsigned int k = 0; k < _gameObjects[SPAWN].size(); k++)
					{
						//If the enemy is at the despawn point with the diamond, remove the diamond and the enemy, Aron
						if ((int)unit->GetTilePosition()._x == (int)_gameObjects[SPAWN][k]->GetTilePosition()._x &&
							(int)unit->GetTilePosition()._y == (int)_gameObjects[SPAWN][k]->GetTilePosition()._y)
						{
							Remove(heldObject);
						}
					}

					if (heldObject != nullptr)
					{
						heldObject->SetPosition(XMFLOAT3(heldObject->GetPosition().x, 0.0f, heldObject->GetPosition().z));
					}

					Remove(g);
					g = nullptr;
					j--;
				}
				else
				{
					float xOffset = abs(g->GetPosition().x - g->GetTilePosition()._x);
					float zOffset = abs(g->GetPosition().z - g->GetTilePosition()._y);
					if (xOffset > 0.99f || zOffset > 0.99f)																		 //If unit is on a new tile	
					{
						_tilemap->RemoveObjectFromTile(g->GetTilePosition(), unit);
						unit->Move();
						_tilemap->AddObjectToTile(g->GetTilePosition(), unit);

						/*if (_tilemap->IsTrapOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y))
						{
							static_cast<Trap*>(_tilemap->GetObjectOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y, TRAP))->Activate(unit);
						}*/
					}
					if (unit->GetType() == GUARD && _tilemap->IsEnemyOnTile(g->GetTilePosition()))
					{
						unit->act(_tilemap->GetObjectOnTile(g->GetTilePosition(), ENEMY));
					}
				}
			}
			else if (g->GetType() == SPAWN)															//Manage enemy spawning
			{
				if (static_cast<SpawnPoint*>(g)->isSpawning())
				{
					if (Add(ENEMY, "enemy_proto", g->GetPosition(), g->GetRotation()))
					{
						((Unit*)_gameObjects[ENEMY].back())->Move();
					}
				}
			}
			//else if (g->GetType() == TRAP)
			//{
			//	Unit* unit = static_cast<Unit*>(_tilemap->GetUnitOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y));
			//	//Enemy walks over trap
			//	if (unit != nullptr)
			//	{
			//		static_cast<Trap*>(g)->Activate(unit);
			//	}
			//}
		}
	}

	/*
	
	if(animations uppdateringar)
		if(active)
			do ljus update

	if(gameobject förflyttning)
		if(active)
			do ljus update
	
	*/

	for (pair<GameObject*, Renderer::Spotlight*> spot : _spotlights)
	{
		if (spot.second->IsActive() && spot.first->IsActive())
		{
			if (spot.second->GetBone() != -1)
			{
				spot.second->SetPositionAndRotation(spot.first->GetAnimation()->GetTransforms()->at(spot.second->GetBone()));
			}
			else
			{
				spot.second->SetPositionAndRotation(spot.first->GetPosition(), spot.first->GetRotation());
			}
		}
	}
}

void ObjectHandler::ReleaseGameObjects()
{
	int debug;
	std::vector<GameObject*> tempVector;

	if (_gameObjects.size() > 0)
	{
		for (int i = 0; i < NR_OF_TYPES; i++)
		{
			for (GameObject* g : _gameObjects[i])
			{
				g->Release();
				delete g;
			}
			_gameObjects[i].clear();
			std::vector<GameObject*>().swap(_gameObjects[i]);
			_gameObjects[i].shrink_to_fit();
			debug = _gameObjects[i].size();
		}
		_gameObjects.clear();
		std::vector<GameObject*>().swap(tempVector);
		_gameObjects.shrink_to_fit();
		debug = _gameObjects.size();
	}
	_idCount = 0;
	_objectCount = 0;
}


/*Make object*/

Architecture * ObjectHandler::MakeFloor(GameObjectFloorInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		FLOOR,
		_assetManager->GetRenderObject(data->_renderObject));

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Architecture * ObjectHandler::MakeWall(GameObjectWallInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		WALL,
		_assetManager->GetRenderObject(data->_renderObject));

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Architecture * ObjectHandler::MakeLoot(GameObjectLootInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Architecture* obj = new Architecture(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		LOOT,
		_assetManager->GetRenderObject(data->_renderObject));

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

SpawnPoint * ObjectHandler::MakeSpawn(GameObjectSpawnInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	SpawnPoint* obj = new SpawnPoint(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		SPAWN,
		_assetManager->GetRenderObject(data->_renderObject),
		180, 2);

	// read more data

	//new SpawnPoint(_idCount, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(type), 180, 2);

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Trap * ObjectHandler::MakeTrap(GameObjectTrapInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Trap* obj = new Trap(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		TRAP,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap,
		SPIKE,
		{ 1,0 },
		data->_cost);

	// Read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Guard * ObjectHandler::MakeGuard(GameObjectGuardInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Guard* obj = new Guard(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		GUARD,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap);

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Enemy * ObjectHandler::MakeEnemy(GameObjectEnemyInfo * data, const XMFLOAT3& position, const XMFLOAT3& rotation)
{
	Enemy* obj = new Enemy(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		ENEMY,
		_assetManager->GetRenderObject(data->_renderObject),
		_tilemap);

	// read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}
