#pragma once

#include <cstdio>
#include <ShlObj.h>

#include "../ObjectHandler.h"
#include "../Grid.h"
#include "AStar.h"
#include "../Controls.h"
#include "../PickingDevice.h"
#include "../System/Camera.h"
//#include "../Renderer/RenderModule.h"

class BaseEdit
{
private:
	AI::AStar*				_aStar;
	System::Controls*		_controls;
	System::Camera*			_camera;
	Tilemap*				_tileMap;
	ObjectHandler*			_objectHandler;
	PickingDevice*			_pickingDevice;
	Grid*					_grid;
	//Renderer::RenderModule*	_renderModule;

	struct ButtonInfo
	{
		int parent;
		bool active = false;
	};

	GameObject* _marker;
	AI::Vec2D* _markedTile;

	System::MouseCoord _mouseCoord;
	int _tileMultiplier;
	int _tilemapHeight, _tilemapWidth;
	std::vector<ButtonInfo> buttonInfo;
	
	// FLAGS
	bool _isSelectionMode;
	bool _isDragAndPlaceMode;
	bool _isPlace;

	void HandleInput();
	void LoadLevel(int levelID);

public:
	BaseEdit();
	~BaseEdit();

	void Initialize(ObjectHandler* objectHandler, System::Controls* controls, PickingDevice* pickingDevice, System::Camera* camera);

	GameObject* GetSelectedObject();
	bool Add(Type type, std::string name);
	bool Delete(Type type);
	bool TypeOn(Type type);
	void DragAndDrop(Type type);
	void DragAndDrop();
	void DragAndPlace(Type type, std::string objectName);
	
	void DragActivate(Type type, std::string objectName);
	void ChangePlaceState();
	bool IsSelection() const;
	bool IsDragAndPlace() const;
	bool IsPlace() const;

	void Update(float deltaTime);
};