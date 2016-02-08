#include "GameLogic.h"
#include <DirectXMath.h>
#include "InputDevice.h"

GameLogic::GameLogic()
{
	_player = nullptr;
	_objectHandler = nullptr;
	_camera = nullptr;
	_controls = nullptr;
	_pickingDevice = nullptr;
	_levelLoad = LevelLoad();
}

GameLogic::~GameLogic()
{
	delete _player;
}

void GameLogic::Initialize(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice)
{
	_objectHandler = objectHandler;
	_camera = camera;
	_controls = controls;
	_pickingDevice = pickingDevice;

	//_objectHandler->LoadLevel(3);
	//Either import the level here or in the LevelEdit.cpp, otherwise the level will be loaded twice
	//System::loadJSON(&_levelLoad, "../../../../StortSpelprojekt/Assets/LevelLoad.json");
	//_objectHandler->LoadLevel(_levelLoad.level);

	_objectHandler->InitPathfinding();

	_player = new Player();
}

void GameLogic::Update(float deltaTime)
{
	HandleInput();
	_objectHandler->Update(deltaTime);
}

void GameLogic::HandleInput()
{
	//Selecting a Unit and moving selected units
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		vector<GameObject*> pickedUnits = _pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, _objectHandler->GetAllByType(GUARD));


		if (pickedUnits.empty())
		{
			if (_player->AreUnitsSelected())
			{
				_player->MoveUnits(_pickingDevice->PickTile(_controls->GetMouseCoord()._pos));
			}
		}
		else
		{
			vector<Unit*> units = _player->GetSelectedUnits();
			for (unsigned int i = 0; i < units.size(); i++)
			{
				units[i]->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			}
			_player->DeselectUnits();


			Unit* unit = (Unit*)pickedUnits[0];
			_player->SelectUnit(unit);

			unit->SetScale(DirectX::XMFLOAT3(1.2f, 1.2f, 1.2f));
		}
	}
	//Deselect Units
	if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		if (_player->AreUnitsSelected())
		{
			vector<Unit*> units = _player->GetSelectedUnits();
			for (unsigned int i = 0; i < units.size(); i++)
			{
				units[i]->SetScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			}
			_player->DeselectUnits();
		}
	}
	
	//Boxselect Units
	if(_controls->IsFunctionKeyDown("MOUSE:BOX_SELECT"))
	{
		_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);
	}

	if (_controls->IsFunctionKeyUp("MOUSE:BOX_SELECT"))
	{
		vector<GameObject*> pickedUnits = _pickingDevice->BoxPickObjects(_controls->GetMouseCoord()._pos, _objectHandler->GetAllByType(GUARD));

		for (unsigned int i = 0; i < pickedUnits.size(); i++)
		{
			_player->SelectUnit((Unit*)pickedUnits[i]);
			pickedUnits[i]->SetScale(DirectX::XMFLOAT3(1.2f, 1.2f, 1.2f));
		}

	}

	//Set Guard Patrol Route if a Guard is Selected
	if (_controls->IsFunctionKeyDown("PLAY:SET_PATROL"))
	{
		if (_player->AreUnitsSelected())
		{
			_player->PatrolUnits(_pickingDevice->PickTile(_controls->GetMouseCoord()._pos));
		}
	}

	/*
	Toggle free camera mode
	*/
	if (_controls->IsFunctionKeyDown("DEBUG:ENABLE_FREECAM"))
	{
		if (_camera->GetMode() == System::LOCKED_CAM)
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::FREE_CAM);
		}
		else
		{
			_controls->ToggleCursorLock();
			_camera->SetMode(System::LOCKED_CAM);
			_camera->SetRotation(DirectX::XMFLOAT3(70, 0, 0));
		}
	}

	/*
	Camera scroll
	*/
	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_IN") &&
			_camera->GetPosition().y > 4.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, -1.0f, 0.0f));
		}
		else if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_OUT") &&
			_camera->GetPosition().y < 12.0f)
		{
			_camera->Move(XMFLOAT3(0.0f, 1.0f, 0.0f));
		}
	}

	/*
	Camera rotation
	*/
	bool isRotating = false;
	XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);
	float rotV = 1.4f;

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_LEFT"))
		{
			rotation.y = 1.0f;
			isRotating = true;
		}
		if (_controls->IsFunctionKeyDown("CAMERA:ROTATE_CAMERA_RIGHT"))
		{
			rotation.y = -1.0f;
			isRotating = true;
		}

		if (isRotating)
		{
			_camera->Rotate(XMFLOAT3((rotation.x * rotV), (rotation.y * rotV), (rotation.z * rotV)));
		}
	}

	//Camera rotation - locked mouse
	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}

	/*
	Camera move
	*/
	XMFLOAT3 right(0.0f, 0.0f, 0.0f);
	XMFLOAT3 forward(0.0f, 0.0f, 0.0f);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01);

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_UP"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
		}

		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_DOWN"))
	{
		if (_camera->GetMode() == System::FREE_CAM)
		{
			forward = _camera->GetForwardVector();
		}
		else if (_camera->GetMode() == System::LOCKED_CAM)
		{
			forward = (Vec3(_camera->GetRightVector()).Cross(Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f))).convertToXMFLOAT());
		}

		forward.x *= -1;
		forward.y *= -1;
		forward.z *= -1;
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_RIGHT"))
	{
		right = _camera->GetRightVector();
		isMoving = true;
	}

	if (_controls->IsFunctionKeyDown("CAMERA:MOVE_CAMERA_LEFT"))
	{
		right = _camera->GetRightVector();
		right.x *= -1;
		right.y *= -1;
		right.z *= -1;
		isMoving = true;
	}

	if (isMoving)
	{
		_camera->Move(XMFLOAT3((forward.x + right.x) * v, (forward.y + right.y) * v, (forward.z + right.z) * v));
	}
	
}