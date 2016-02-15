#pragma once
#include "ObjectHandler.h"
#include "Camera.h"
#include "Controls.h"
#include "PickingDevice.h"
#include "Player.h"
#include "JsonStructs.h"
#include "..\System\JsonParser.h"
#include "UITree.h"
#include "AssetManager.h"

class GameLogic
{
private:
	ObjectHandler*			_objectHandler;
	System::Camera*			_camera;
	System::Controls*		_controls;
	PickingDevice*			_pickingDevice;
	Player*					_player;
	GUI::UITree*			_uiTree;
	AssetManager*			_assetManager;
	ID3D11ShaderResourceView* _guardTexture;
	bool					_gameDone;

	void HandleInput(float deltaTime);
public:
	GameLogic(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice, GUI::UITree* uiTree, AssetManager* assetManager);
	~GameLogic();
	void Update(float deltaTime);
	bool IsGameDone()const;
};



