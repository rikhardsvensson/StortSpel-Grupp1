#pragma once

#include <cstdio>
#include <ShlObj.h>
#include "../UIHandler.h"
#include "../ObjectHandler.h"
#include "../System/InputDevice.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"

class LevelEdit
{
private:
	AI::AStar*				_aStar;
	System::InputDevice*	_inputDevice;
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;
	UIHandler*				_uiHandler;
	Grid*					_grid;

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

	struct ButtonInfo
	{
		int parent;
		bool active = false;
	};

	GameObject* _marker;
	GameObject* _lastSelected;

	System::MouseCoord _mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	bool _isSelectionMode = true;
	std::vector<ButtonInfo> buttonInfo;
	
	
	void DragAndDrop(Type type);
	void DragAndDrop();

	void HandleInput();



	void HandleHUD();
	void LoadLevel(int levelID);
	void InitNewLevel();
	void ResetSelectedObj();
	void HandleSelected();

	void ExportLevel();

public:
	LevelEdit();
	~LevelEdit();

	GameObject* GetSelectedObject();
	void Add(Type type, int renderObjectID);
	bool Delete(Type type);
	bool Marked(Type type);
	void Initialize(ObjectHandler* objectHandler, 
		System::InputDevice* inputDevice, 
		System::Controls* controls, 
		PickingDevice* pickingDevice, 
		System::Camera* camera, 
		UIHandler* uiHandler);
	void Update(float deltaTime);
};