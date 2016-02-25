#include "GameLogic.h"
#include <DirectXMath.h>
#include "InputDevice.h"

GameLogic::GameLogic(ObjectHandler* objectHandler, System::Camera* camera, System::Controls* controls, PickingDevice* pickingDevice, GUI::UITree* uiTree, AssetManager* assetManager, System::SettingsReader* settingsReader , System::SoundModule* soundModule)
{
	_objectHandler = objectHandler;
	_camera = camera;
	_controls = controls;
	_pickingDevice = pickingDevice;
	_settingsReader = settingsReader;

	_player = new Player(objectHandler);
	_objectHandler->InitPathfinding();
	_uiTree = uiTree;
	_assetManager = assetManager;
	_guardTexture = _assetManager->GetTexture("../Menues/PlacementStateGUI/units/Guardbutton1.png")->_data;
	_uiTree->GetNode("winscreen")->SetHidden(true);
	_uiTree->GetNode("losescreen")->SetHidden(true);
	_gameOver = false;
	_buttonReady = 3999.0f; //TODO: Replace with timer /Rikhard
	_returnToMenu = false;

	Level::LevelHeader* currentLevelHeader = _objectHandler->GetCurrentLevelHeader();
	_gameMode = currentLevelHeader->_gameMode;
	_surviveForSeconds = currentLevelHeader->_surviveForSeconds;
	
	_soundModule = soundModule;
	_soundModule->AddSound("Assets/Sounds/unit_select", 0.5f, 1.0f, true, false);
	_soundModule->AddSound("Assets/Sounds/unit_move", 0.5f, 1.0f, true, false);
}

GameLogic::~GameLogic()
{
	delete _player;
	_player = nullptr;
}

void GameLogic::Update(float deltaTime)
{
	CheckGameStatus();
	if (!_returnToMenu)
	{
		HandleInput(deltaTime);
		_objectHandler->Update(deltaTime);
	}
	if(_gameOver)
	{
		HandleWinLoseDialog(deltaTime);
	}
	_uiTree->GetNode("objectivetext")->SetText(L"Defeat the intruders! \n" + std::to_wstring(_objectHandler->GetAllByType(ENEMY)->size()) + L" enemies still remain.");
}

bool GameLogic::GoToMenu() const
{
	return _returnToMenu;
}

/*
Private functions
*/
void GameLogic::HandleInput(float deltaTime)
{
	/*
	Select units
	*/
	HandleUnitSelect();

	/*
	Show selected units on the GUI
	*/
	ShowSelectedInfo();

	/*
	Handle patrol
	*/
	HandlePatrol();

	/*
	Move units
	*/
	HandleUnitMove();

}

void GameLogic::HandleUnitSelect()
{
	/*
	Select units
	*/
	if (_controls->IsFunctionKeyDown("MOUSE:SELECT"))
	{
		_pickingDevice->SetFirstBoxPoint(_controls->GetMouseCoord()._pos);
	}

	if (_controls->IsFunctionKeyUp("MOUSE:SELECT"))
	{
		//Deselect everything first.
		_player->DeselectUnits();

		//Check if we picked anything
		vector<GameObject*> pickedUnits = _pickingDevice->PickObjects(_controls->GetMouseCoord()._pos, *_objectHandler->GetAllByType(GUARD));

		//if units selected
		if (pickedUnits.size() > 0)
		{
			//select
			for (unsigned int i = 0; i < pickedUnits.size(); i++)
			{
				_player->SelectUnit((Unit*)pickedUnits[i]);
			}

			//Play "hm" sound
			_soundModule->Play("Assets/Sounds/unit_select");
		}
	}

	//Update Icon over Selected Guards
	for (auto u : _player->GetSelectedUnits())
	{
		XMFLOAT3 pos = u->GetPosition();
		pos.y += 2.5f;
		ParticleUpdateMessage* msg = new ParticleUpdateMessage(u->GetID(), true, pos);
		_objectHandler->GetParticleEventQueue()->Insert(msg);
	}
}

void GameLogic::HandlePatrol()
{
	//Set Guard Patrol Route if a Guard is Selected
	if (_controls->IsFunctionKeyDown("PLAY:SET_PATROL"))
	{
		if (_player->AreUnitsSelected())
		{
			AI::Vec2D tilePos = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);

			if (_objectHandler->GetTileMap()->IsFloorOnTile(tilePos))
			{
				_player->PatrolUnits(tilePos);
			}	
		}
	}
}

void GameLogic::HandleUnitMove()
{
	//Move units
	if (_controls->IsFunctionKeyDown("MOUSE:DESELECT"))
	{
		AI::Vec2D selectedTile = _pickingDevice->PickTile(_controls->GetMouseCoord()._pos);
		vector<Unit*> units = _player->GetSelectedUnits();

		//if one unit
		if (units.size() == 1 && selectedTile == units.at(0)->GetTilePosition() && units.at(0)->GetMoveState() == Unit::MoveState::IDLE)
		{
				//Check which direction he should be pointing
				AI::Vec2D direction = _pickingDevice->PickDirection(_controls->GetMouseCoord()._pos, _objectHandler->GetTileMap());

				//Change direction
				units.at(0)->SetDirection(direction);
		}
		else if(units.size() > 0)
		{
			_player->MoveUnits(selectedTile);
			_soundModule->Play("Assets/Sounds/unit_move");
		}
	}
}

void GameLogic::ShowSelectedInfo()
{
	/*
	Show selected units on the GUI
	*/
	if (_player->AreUnitsSelected())
	{
		vector<Unit*> units = _player->GetSelectedUnits();
		int nrOfUnits = _player->GetNumberOfSelectedUnits();
		int healthSum = 0;

		_uiTree->GetNode("unitinfocontainer")->SetHidden(false);

		//TODO: Actually check if the units are guards //Mattias
		_uiTree->GetNode("unitinfotext")->SetText(L"Guard");
		_uiTree->GetNode("unitpicture")->SetTexture(_guardTexture);

		//Calculate health sum
		for (auto& i : units)
		{
			healthSum += i->GetHealth();
		}
		//TODO: Health is always 1? //Mattias
		_uiTree->GetNode("unithealth")->SetText(std::to_wstring(healthSum * 100) + L"%");

		//Show the number of units selected
		if (nrOfUnits > 1)
		{
			_uiTree->GetNode("unitnumber")->SetHidden(false);
			_uiTree->GetNode("unitnumber")->SetText(L"x " + std::to_wstring(nrOfUnits));
		}
		else
		{
			_uiTree->GetNode("unitnumber")->SetHidden(true);
		}
	}
	else
	{
		_uiTree->GetNode("unitinfocontainer")->SetHidden(true);
	}
}

void GameLogic::HandleWinLoseDialog(float deltaTime)
{
	if (_objectHandler->GetAllByType(LOOT)->size() <= 0)				//You lost
	{
		_uiTree->GetNode("losescreen")->SetHidden(false);
		System::MouseCoord coord = _controls->GetMouseCoord();
		int time = _buttonReady / 1000;
		if (time > 0)
		{
			_uiTree->GetNode("losebuttontext")->SetText(L".." + to_wstring(time));
		}
		else
		{
			_uiTree->GetNode("losebuttontext")->SetText(L"Continue");
		}
		if (time <= 0 && _uiTree->IsButtonColliding("losebutton", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_returnToMenu = true;
		}
		_buttonReady -= deltaTime;
	}
	else if (_objectHandler->GetAllByType(ENEMY)->size() <= 0)		//You won
	{
		System::MouseCoord coord = _controls->GetMouseCoord();
		_uiTree->GetNode("winscreen")->SetHidden(false);
		int time = _buttonReady / 1000;
		if (time > 0)
		{
			_uiTree->GetNode("winbuttontext")->SetText(L".." + to_wstring(time));
		}
		else
		{
			_uiTree->GetNode("winbuttontext")->SetText(L"Continue");
		}
		if (time <= 0 && _uiTree->IsButtonColliding("winbutton", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			_returnToMenu = true;
			_settingsReader->GetProfile()->_level += 1;
			_settingsReader->ApplyProfileSettings();
		}
		_buttonReady -= deltaTime;
	}
}

bool GameLogic::CheckGameStatus()
{
	if (_objectHandler->GetAllByType(LOOT)->size() <= 0 && _objectHandler->GetRemainingToSpawn() <= 0 
		|| _objectHandler->GetAllByType(ENEMY)->size() <= 0 && _objectHandler->GetRemainingToSpawn() <= 0)
	{
		_gameOver = true;
	}
	else
	{
		_gameOver = false;
	}
	return _gameOver;
}