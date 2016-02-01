#include "LevelSelectState.h"



LevelSelectState::LevelSelectState(System::Controls* controls, ObjectHandler* objectHandler, System::Camera* camera, PickingDevice* pickingDevice, const std::string& filename, AssetManager* assetManager, FontWrapper* fontWrapper, int width, int height) :
	BaseState(controls, objectHandler, camera, pickingDevice, filename, "LEVELSELECT", assetManager, fontWrapper, width, height)
{

}


LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Update(float deltaTime)
{
	if (_controls->IsFunctionKeyDown("DEBUG:RELOAD_GUI"))
	{
		_uiTree.ReloadTree("../../../../StortSpelprojekt/Assets/gui.json", "LEVELSELECT");
	}
}

void LevelSelectState::OnStateEnter()
{
}

void LevelSelectState::OnStateExit()
{
}
