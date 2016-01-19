#pragma once

#include "BaseState.h"

class MenuState : public BaseState
{
private:

public:
	MenuState(InitVar initVar, const std::string& filename);
	~MenuState();

	void Update(float deltaTime);
	void OnStateEnter();
	void OnStateExit();
};
