#pragma once
#include "BaseState.h"
class LevelSelectState :
	public BaseState
{
public:
	LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, System::Settings* settings, System::SoundModule* soundModule);
	~LevelSelectState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};

