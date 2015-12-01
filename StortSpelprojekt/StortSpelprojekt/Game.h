#pragma once

#include "stdafx.h"
#include "StateMachine.h"
#include "Window.h"
#include "RenderModule.h"
#include "UIHandler.h"
#include "ObjectHandler.h"

#include "Trap.h"

class Game
{
private:

	StateMachine			_SM;
	System::Window*			_window;
	Renderer::RenderModule* _renderModule;

	UIHandler*				_UI;
	ObjectHandler* _objectHandler;

	void HandleInput();
	void Update();
	void Render();

public:

	Game(HINSTANCE hInstance, int nCmdShow);
	~Game();

	int Run();
};