#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "UIHandler.h"
#include "ObjectHandler.h"
#include "InputDevice.h"
#include "StateMachine/InitVar.h"
#include "AStar.h"

class LevelEdit
{
private:
	InitVar*				_initVar;
	AI::AStar*				_aStar;
	Tilemap*				_tileMap;


	

	struct LevelHeader
	{
		int _version;
		int _levelSizeX;
		int _levelSizeY;
		int _nrOfGameObjects;
	};

	struct MapData
	{
		int _posX;
		int _posZ;
		float _rotY;
		int _tileType;
	};

	XMFLOAT3 _lastObjPosition = XMFLOAT3(0.0, 0.0, 0.0);
	GameObject* _selectedObj;
	GameObject* _lastSelected;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;

	void HandleInput();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void DeleteObject();

	void ExportLevel();
	void SelectObject(GameObject* selectedObject);
	void MoveSelectedObject();

public:
	LevelEdit();
	~LevelEdit();

	void Update(float deltaTime);
	void Initialize(InitVar* initVar);
};