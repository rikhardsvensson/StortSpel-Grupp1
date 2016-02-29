#include "TutorialLogic.h"

TutorialLogic::TutorialLogic(GUI::UITree* uiTree, System::Controls* controls)
{
	_uiTree = uiTree;
	_controls = controls;
	_gold = 500;
	_sCameraPlaced = false;
	_tutorialCompleted = false;
	ResetUiTree();
	_time = 0;
	_light = false;
}

TutorialLogic::~TutorialLogic()
{
	_uiTree->GetNode("welcome")->SetHidden(true);
	_uiTree->GetNode("controls")->SetHidden(true);
	_uiTree->GetNode("objective")->SetHidden(true);
	_uiTree->GetNode("guardexplained")->SetHidden(true);
	_uiTree->GetNode("guardplace")->SetHidden(true);
	_uiTree->GetNode("anvilexplained")->SetHidden(true);
	_uiTree->GetNode("anvilplace")->SetHidden(true);
	_uiTree->GetNode("teslaexplained")->SetHidden(true);
	_uiTree->GetNode("teslaplace")->SetHidden(true);
	_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
	_uiTree->GetNode("securitycameraplace")->SetHidden(true);
	_uiTree->GetNode("budgetexplained")->SetHidden(true);
	_uiTree->GetNode("playexplained")->SetHidden(true);
}

void TutorialLogic::ResetUiTree()
{
	_tutorialCompleted = false;
	_currentStage = WELCOME;
	_uiTree->GetNode("welcome")->SetHidden(false);
	_uiTree->GetNode("controls")->SetHidden(true);
	_uiTree->GetNode("objective")->SetHidden(true);
	_uiTree->GetNode("guardexplained")->SetHidden(true);
	_uiTree->GetNode("guardplace")->SetHidden(true);
	_uiTree->GetNode("anvilexplained")->SetHidden(true);
	_uiTree->GetNode("anvilplace")->SetHidden(true);
	_uiTree->GetNode("teslaexplained")->SetHidden(true);
	_uiTree->GetNode("teslaplace")->SetHidden(true);
	_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
	_uiTree->GetNode("securitycameraplace")->SetHidden(true);
	_uiTree->GetNode("budgetexplained")->SetHidden(true);
	_uiTree->GetNode("playexplained")->SetHidden(true);
}

bool TutorialLogic::Update(float deltaTime, PlayerInfo playerProfile)
{
	
	System::MouseCoord coord = _controls->GetMouseCoord();
	_time += deltaTime;
	//icon blink speed
	if (_time > 1200)
	{
		_time = 0;
		_light = !_light;
	}
	/*
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree->ReloadTree("../../../../StortSpelprojekt/Assets/GUI/placement.json");
		ResetUiTree();
	}

	switch (_currentStage)
	{
	case WELCOME:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = CONTROLS;
			_uiTree->GetNode("welcome")->SetHidden(true);
			_uiTree->GetNode("controls")->SetHidden(false);
		}
		break;
	}
	case CONTROLS:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = OBJECTIVE;
			_uiTree->GetNode("controls")->SetHidden(true);
			_uiTree->GetNode("objective")->SetHidden(false);
		}
		break;
	}
	case OBJECTIVE:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARDEXPLAINED;
			_uiTree->GetNode("objective")->SetHidden(true);
			_uiTree->GetNode("guardexplained")->SetHidden(false);
		}
		break;
	}
	case GUARDEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = GUARDPLACE;
			_uiTree->GetNode("guardexplained")->SetHidden(true);
			_uiTree->GetNode("guardplace")->SetHidden(false);
		}
		break;
	}
	case GUARDPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}


		if (_uiTree->IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = System::Type::GUARD;
			toPlace._name = "guard_proto";

			//If we placed a guard.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				//baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				//_gold -= toPlace._goldCost;
				//_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				//toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

				//_currentStage = ANVILEXPLAINED;
				//_uiTree->GetNode("guardplace")->SetHidden(true);
				//_uiTree->GetNode("anvilexplained")->SetHidden(false);
				//_uiTree->GetNode("Guard")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
			}
		}
		break;
	}
	case ANVILEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = ANVILPLACE;
			_uiTree->GetNode("anvilexplained")->SetHidden(true);
			_uiTree->GetNode("anvilplace")->SetHidden(false);
		}
		break;
	}
	case ANVILPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}
		//Check mouse pos with button
		if (_uiTree->IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = System::Type::TRAP;
			toPlace._name = "trap_proto";

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
					toPlace._subType = TrapType::SPIKE;
					baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
					_gold -= toPlace._goldCost;
					_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
					toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

					_currentStage = TESLAEXPLAINED;
					_uiTree->GetNode("anvilplace")->SetHidden(true);
					_uiTree->GetNode("teslaexplained")->SetHidden(false);
					_uiTree->GetNode("AnvilTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
			}
		}
		break;
	}
	case TESLAEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = TESLAPLACE;
			_uiTree->GetNode("teslaexplained")->SetHidden(true);
			_uiTree->GetNode("teslaplace")->SetHidden(false);
		}
		break;
	}
	case TESLAPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}

		if (_uiTree->IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = System::Type::TRAP;
			toPlace._name = "tesla_trap";
			toPlace._subType = TrapType::TESLACOIL;

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();

				_currentStage = SECURITY(System::TypeEXPLAINED;
				_uiTree->GetNode("teslaplace")->SetHidden(true);
				_uiTree->GetNode("securitycameraexplained")->SetHidden(false);
				_uiTree->GetNode("TeslaTrap")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
			}
			}
			break;
		}
	case SECURITYCAMERAEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = SECURITYCAMERAEXPLAINED;
			_uiTree->GetNode("securitycameraexplained")->SetHidden(true);
			_uiTree->GetNode("securitycameraplace")->SetHidden(false);
		}
		break;
	}
	case SECURITYCAMERAPLACE:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}

		//Check mouse pos with button
		if (_uiTree->IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			// Temp, should be replaced with blueprint
			toPlace._type = System::CAMERA;
			toPlace._name = "camera_proto";

			//If we placed an anvil.
			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
				_sCameraPlaced = true;
				_uiTree->GetNode("Camera")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
			}
		}
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE") && _sCameraPlaced)
		{
			_currentStage = BUDGETEXPLAINED;
			_uiTree->GetNode("securitycameraplace")->SetHidden(true);
			_uiTree->GetNode("budgetexplained")->SetHidden(false);
		}
		break;
	}
	case BUDGETEXPLAINED:
	{
		if (_controls->IsFunctionKeyDown("MENU:CONTINUE"))
		{
			_currentStage = PLAYEXPLAINED;
			_uiTree->GetNode("budgetexplained")->SetHidden(true);
			_uiTree->GetNode("playexplained")->SetHidden(false);
		}


		break;
	}
	case PLAYEXPLAINED:
	{
		//Blink button
		if (_light)
		{
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
		}
		else
		{
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}

		if (_uiTree->IsButtonColliding("Play", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			playerProfile._firstTime = false;
			_tutorialCompleted = true;
			_uiTree->GetNode("Play")->SetColorOffset(DirectX::XMFLOAT4(0, 0, 0, 1.0f));
		}

		
		//If we want to be able to place even more traps before hitting play.
		//Guards
		if (_uiTree->IsButtonColliding("Guard", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			toPlace._type = Type::GUARD;
			toPlace._name = "guard_proto";

			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
			}
		}
		if (_uiTree->IsButtonColliding("AnvilTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			toPlace._type = Type::TRAP;
			toPlace._name = "trap_proto";

			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				toPlace._subType = TrapType::SPIKE;
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
			}
		}

		if (_uiTree->IsButtonColliding("TeslaTrap", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			toPlace._type = Type::TRAP;
			toPlace._name = "tesla_trap";

			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				toPlace._subType = TrapType::TESLACOIL;
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
			}
		}
		if (_uiTree->IsButtonColliding("Camera", coord._pos.x, coord._pos.y) && _controls->IsFunctionKeyDown("MOUSE:SELECT"))
		{
			toPlace._type = Type::(System::Type;
			toPlace._name = "camera_proto";

			if (baseEdit->IsSelection() && !baseEdit->IsPlace())
			{
				baseEdit->DragActivate(toPlace._type, toPlace._name, toPlace._subType);
				_gold -= toPlace._goldCost;
				_uiTree->GetNode("BudgetValue")->SetText(to_wstring(_gold));
				toPlace._blueprintID = baseEdit->GetSelectedObject()->GetID();
			}
		}
		
		break;
	}
	default:
		break;
	}
	}
	*/
	return _tutorialCompleted;
}


bool TutorialLogic::IsTutorialCompleted() const
{
	return _tutorialCompleted;
}