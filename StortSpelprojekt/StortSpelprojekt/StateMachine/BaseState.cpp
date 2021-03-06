#include "BaseState.h"

State BaseState::_newStateRequest;
State BaseState::_oldState;
//TutorialState BaseState::_tutorialState;

BaseState::BaseState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::SettingsReader* settingsReader, System::SoundModule* soundModule)
	: _uiTree(filename, assetManager, fontWrapper, settingsReader)
{
	_controls			= controls;
	_objectHandler		= objectHandler;
	_assetManager		= assetManager;
	_camera				= camera;
	_pickingDevice		= pickingDevice;
	_newStateRequest	= SPLASHSTATE;
	_oldState			= EXITSTATE;
	_soundModule		= soundModule;
	_settingsReader		= settingsReader;
}

BaseState::~BaseState()
{}

void BaseState::ChangeState(State newState)
{
	_oldState = _newStateRequest;
	_newStateRequest = newState;
	ResetMouse();
}

State BaseState::GetNewStateRequest()const
{
	return _newStateRequest;
}

State BaseState::GetOldState()const
{
	return _oldState;
}

void BaseState::HandleCam(float deltaTime)
{
	HandleCamMode();
	HandleCamZoom();
	HandleCamRot();
	HandleCamMove(deltaTime);
}

void BaseState::HandleCamMode()
{
	/*
	Toggle free camera mode
	*/
#ifdef _DEBUG
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
			_camera->SetRotation(DirectX::XMFLOAT3(60, 0, 0));
		}
	}
#endif 
}

void BaseState::HandleCamZoom()
{
	/*
	Camera scroll
	*/
	const float ZOOM_MAX = 12.0f;
	const float ZOOM_MIN = 5.0f;
	float camStepSize = 2.0f;

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_IN") &&
			(_camera->GetPosition().y) > ZOOM_MIN)
		{
			_camera->Move(_camera->GetForwardVector(), camStepSize);
		}
		else if (_controls->IsFunctionKeyDown("CAMERA:ZOOM_CAMERA_OUT") &&
			(_camera->GetPosition().y) < ZOOM_MAX)
		{
			XMFLOAT3 negForward = _camera->GetForwardVector();
			negForward.x *= -1.0f;
			negForward.y *= -1.0f;
			negForward.z *= -1.0f;
			_camera->Move(negForward, camStepSize);
		}
	}
}

void BaseState::HandleCamRot()
{
	/*
	Camera rotation
	*/

	if (_camera->GetMode() == System::LOCKED_CAM)
	{
		bool isRotating = false;
		float rotV = 1.4f;
		XMFLOAT3 rotation(0.0f, 0.0f, 0.0f);

		Vec3 p0 = Vec3(XMFLOAT3(1.0f, 0.0f, 0.0f));
		Vec3 p1 = Vec3(XMFLOAT3(0.0f, 0.0f, 1.0f));
		Vec3 n = Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f));
		Vec3 camPos = Vec3(_camera->GetPosition());
		Vec3 camDir = Vec3(_camera->GetForwardVector());

		float distance = (((p0 - camPos).Dot(n)) / (camDir.Dot(n)));
		_camera->Move(camDir.convertToXMFLOAT(), distance);

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

		camDir = _camera->GetForwardVector();
		_camera->Move((camDir * -1).convertToXMFLOAT(), distance);
	}


	//Camera rotation - locked mouse
	if (_controls->CursorLocked())
	{
		XMFLOAT3 rotation = _camera->GetRotation();
		rotation.x += _controls->GetMouseCoord()._deltaPos.y / 10.0f;
		rotation.y += _controls->GetMouseCoord()._deltaPos.x / 10.0f;

		_camera->SetRotation(rotation);
	}
}

void BaseState::HandleCamMove(float deltaTime)
{
	/*
	Camera move
	*/
	int width = _objectHandler->GetTileMap()->GetWidth();
	int height = _objectHandler->GetTileMap()->GetHeight();

	const float LIMIT_UPPER = height * 1.2f;
	const float LIMIT_LOWER = height * -0.2f;
	const float LIMIT_RIGHT = width * 1.2f;
	const float LIMIT_LEFT = width * -0.2f;

	XMFLOAT3 right(0.0f, 0.0f, 0.0f);
	XMFLOAT3 forward(0.0f, 0.0f, 0.0f);
	bool isMoving = false;
	float v = 0.06f + (_camera->GetPosition().y * 0.01f);

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
		_camera->Move(XMFLOAT3((forward.x + right.x) * v, (forward.y + right.y) * v, (forward.z + right.z) * v), deltaTime * 0.1f);

		if (_camera->GetMode() == System::LOCKED_CAM)
		{
			//Calculate distance to plane
			Vec3 p0 = Vec3(XMFLOAT3(1.0f, 0.0f, 0.0f));
			Vec3 p1 = Vec3(XMFLOAT3(0.0f, 0.0f, 1.0f));
			Vec3 n = Vec3(XMFLOAT3(0.0f, 1.0f, 0.0f));
			Vec3 camPos = Vec3(_camera->GetPosition());
			Vec3 camDir = Vec3(_camera->GetForwardVector());
			float distance = (((p0 - camPos).Dot(n)) / (camDir.Dot(n)));

			//Move camera to plane
			_camera->Move(camDir.convertToXMFLOAT(), distance);

			XMFLOAT3 campos = _camera->GetPosition();
			//Checks if out of bounds
			_camera->SetPosition(XMFLOAT3(
				max(campos.x, LIMIT_LEFT),
				campos.y,
				max(campos.z, LIMIT_LOWER)));

			campos = _camera->GetPosition();

			_camera->SetPosition(XMFLOAT3(
				min(campos.x, LIMIT_RIGHT),
				campos.y,
				min(campos.z, LIMIT_UPPER)));

			//Move camera back
			_camera->Move((camDir * -1).convertToXMFLOAT(), distance);
		}
	}
}

void BaseState::HandleButtonHighlight(const System::MouseCoord& coord)
{
	for (auto& n : _buttonHighlights)
	{
		if (_uiTree.IsButtonColliding(n._triggerNode, coord))
		{
			n._highlightNode->SetColorOffset(n._color);
		}
		else
		{
			n._highlightNode->SetColorOffset(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		}
	}
}

GUI::UITree* BaseState::GetUITree()
{
	return &_uiTree;
}

void BaseState::Resize(System::Settings* settings)
{
	_uiTree.Resize(settings);
}

void BaseState::ResetMouse()
{
	if (_camera->GetMode() != System::LOCKED_CAM)
	{
		_controls->ToggleCursorLock();
		_camera->SetMode(System::LOCKED_CAM);
		_camera->SetRotation(DirectX::XMFLOAT3(70, 0, 0));
	}
}