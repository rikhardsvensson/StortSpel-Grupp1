#include "ObjectHandler.h"




ObjectHandler::ObjectHandler(ID3D11Device* device, AssetManager* assetManager, GameObjectInfo* data)
{
	_idCount = 0;
	_assetManager = assetManager;
	_tilemap = nullptr;
	_gameObjectData = data;

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		_gameObjects.push_back(std::vector<GameObject*>());
	}
	ActivateTileset("default2");
}

ObjectHandler::~ObjectHandler()
{
	Release();
	delete _tilemap;
	delete _gameObjectData;
}

void ObjectHandler::ActivateTileset(string name)
{
	Release();
	_assetManager->ActivateTileset(name);

	for (uint a = 0; a < Type::NR_OF_TYPES; a++)
	{
		for (uint i = 0; i < _gameObjectData->_objects[a]->size(); i++)
		{
			_gameObjectData->_objects[a]->at(i)->_renderObject = _assetManager->GetRenderObjectByType((Type)a, i);
		}
	}
}



bool ObjectHandler::Add(Type type, int index, XMFLOAT3 position, XMFLOAT3 rotation)
{
	GameObject* object = nullptr;

	switch (type)
	{
	case FLOOR:
		object = MakeFloor(_gameObjectData->Floors(index), position, rotation);
		break;
	case WALL:
		object = MakeWall(_gameObjectData->Walls(index), position, rotation);
		break;
	case LOOT:
		object = MakeLoot(_gameObjectData->Loot(index), position, rotation);
		break;
	case SPAWN:
		object = MakeSpawn(_gameObjectData->Spawns(index), position, rotation);
		//new SpawnPoint(_idCount, position, rotation, AI::Vec2D((int)position.x, (int)position.z), type, _assetManager->GetRenderObject(type), 180, 2);
		break;
	case ENEMY:
		object = MakeEnemy(_gameObjectData->Enemies(index), position, rotation);
		break;
	case GUARD:
		object = MakeGuard(_gameObjectData->Guards(index), position, rotation);
		break;
	case TRAP:
		object = MakeTrap(_gameObjectData->Traps(index), position, rotation);
		break;
	default:	
		break;
	}


	if (object != nullptr)
	{
		_idCount++;
		_gameObjects[type].push_back(object);
		_objectCount++;
		return true;
	}

	return false;
}

bool ObjectHandler::Add(Type type, std::string name, XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	for (int i = 0; i < _gameObjectData->_objects[type]->size(); i++)
	{
		if (_gameObjectData->_objects[type]->at(i)->_name == name)
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
	list._renderObject = _assetManager->GetRenderObject(renderObjectID);;

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
	return _gameObjectData;
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
	if (_tilemap != nullptr)
	{
		delete _tilemap;
	}
	_tilemap = tilemap;
}

void ObjectHandler::MinimizeTileMap()
{
	int minX = 0, maxX;
	for (int y = 0; y < _tilemap->GetHeight(); y++)
	{
		for (int x = 0; x < _tilemap->GetWidth(); x++)
		{
			// Check if something is on the tile
			// if true set minX = x and break
		}
		for (int x = _tilemap->GetWidth() - 1; x > 1; x--)
		{
			// Check if something is on the tile
			// if true set maxX = x and break
		}
	}

	// Do for Y
}

void ObjectHandler::EnlargeTilemap(int offset)
{
	if (offset > 0)
	{
		int o = 2 * offset;
		Tilemap* large = new Tilemap(_tilemap->GetWidth() + o, _tilemap->GetHeight() + o);

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
	}
}




bool ObjectHandler::LoadLevel(int lvlIndex)
{
	int dimX, dimY;
	vector<GameObjectData> gameObjectData;
	bool result = _assetManager->ParseLevel(lvlIndex, gameObjectData, dimX, dimY);
	if (result)
	{
		delete _tilemap;
		_tilemap = new Tilemap(dimX, dimY);

		for (auto i : gameObjectData)
		{
		Add((Type)i._tileType, 0, DirectX::XMFLOAT3(i._posX, 0, i._posZ), DirectX::XMFLOAT3(0, i._rotY, 0));
		}
	}
	return result;
}

void ObjectHandler::InitPathfinding()
{
	for (vector<GameObject*>::iterator i = _gameObjects[ENEMY].begin(); i != _gameObjects[ENEMY].end(); i++)
	{
		Unit* unit = dynamic_cast<Unit*>((*i));
		//unit->CheckAllTiles();
		unit->Move();
	}
	for (vector<GameObject*>::iterator i = _gameObjects[GUARD].begin(); i != _gameObjects[GUARD].end(); i++)
	{

		Unit* unit = dynamic_cast<Unit*>((*i));
		//unit->CheckAllTiles();
		unit->Move();
	}
}

void ObjectHandler::Update(float deltaTime)
{
	//Update all objects gamelogic

	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (int j = 0; j < _gameObjects[i].size(); j++)
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
				_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, g);
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
					//TODO: drop held object and set its tile position --Victor
					if (heldObject != nullptr)
					{
						Remove(heldObject);
					}
					Remove(g);
					g = nullptr;
					j--;
				}
				else
				{
					float xOffset = abs(g->GetPosition().x - g->GetTilePosition()._x);
					float zOffset = abs(g->GetPosition().z - g->GetTilePosition()._y);
					if (xOffset > 0.99 || zOffset > 0.99)																		 //If unit is on a new tile	
					{
						_tilemap->RemoveObjectFromTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);
						unit->Move();
						_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, unit);

						if (_tilemap->IsTrapOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y))
						{
							static_cast<Trap*>(_tilemap->GetObjectOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y, TRAP))->Activate(unit);
						}
					}
					if (unit->GetType() == GUARD && _tilemap->IsEnemyOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y))
					{
						unit->act(_tilemap->GetObjectOnTile(g->GetTilePosition()._x, g->GetTilePosition()._y, ENEMY));
					}
				}
			}
			else if (g->GetType() == SPAWN)															//Manage enemy spawning
			{
				if (static_cast<SpawnPoint*>(g)->isSpawning())
				{
					GameObject* object = new Enemy(_idCount, g->GetPosition(), g->GetRotation(), g->GetTilePosition(), ENEMY, _assetManager->GetRenderObject(ENEMY), _tilemap);
					if (_tilemap->AddObjectToTile(g->GetTilePosition()._x, g->GetTilePosition()._y, object))
					{
						_gameObjects[ENEMY].push_back(object);
						_objectCount++;
						_idCount++;
						static_cast<Unit*>(object)->Move();
					}
					else
					{
						delete object;
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
}

void ObjectHandler::Release()
{
	for (int i = 0; i < NR_OF_TYPES; i++)
	{
		for (GameObject* g : _gameObjects[i])
		{
			g->Release();
			delete g;
		}
		_gameObjects[i].clear();
	}
	_idCount = 0;
	_objectCount = 0;
}


/*Make object*/

Architecture * ObjectHandler::MakeFloor(GameObjectFloorInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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

Architecture * ObjectHandler::MakeWall(GameObjectWallInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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

Architecture * ObjectHandler::MakeLoot(GameObjectLootInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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

SpawnPoint * ObjectHandler::MakeSpawn(GameObjectSpawnInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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

Trap * ObjectHandler::MakeTrap(GameObjectTrapInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
{
	Trap* obj = new Trap(
		_idCount,
		position,
		rotation,
		AI::Vec2D((int)position.x, (int)position.z),
		TRAP,
		_assetManager->GetRenderObject(data->_renderObject),
		data->_cost);

	// Read more data

	if (!_tilemap->AddObjectToTile((int)position.x, (int)position.z, obj))
	{
		delete obj;
		obj = nullptr;
	}

	return obj;
}

Guard * ObjectHandler::MakeGuard(GameObjectGuardInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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

Enemy * ObjectHandler::MakeEnemy(GameObjectEnemyInfo * data, XMFLOAT3 position, XMFLOAT3 rotation)
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
