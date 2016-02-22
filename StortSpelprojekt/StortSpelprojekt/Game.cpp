#include "Game.h"
#include <stdexcept>
#include <DirectXMath.h>
#include <sstream>
#include <random>

Game::Game(HINSTANCE hInstance, int nCmdShow):
	_settingsReader("Assets/settings.xml", "Assets/profile.xml")
{
	srand(time(NULL));
	System::Settings* settings = _settingsReader.GetSettings();

	_gameHandle = this;
	_window = new System::Window("Amazing game", hInstance, settings, WndProc);


	_timer = System::Timer();

	_renderModule = new Renderer::RenderModule(_window->GetHWND(), settings);
	_particleHandler = new Renderer::ParticleHandler(_renderModule->GetDevice(), _renderModule->GetDeviceContext());
	
	_assetManager = new AssetManager(_renderModule->GetDevice());
	_controls = new System::Controls(_window->GetHWND());
	_fontWrapper = new FontWrapper(_renderModule->GetDevice(), L"Assets/Fonts/Calibri.ttf", L"Calibri");

	_camera = new System::Camera(settings);
	_camera->SetPosition(XMFLOAT3(3, 20, 0));
	_camera->SetRotation(XMFLOAT3(60, 0, 0));

	GameObjectDataLoader gameObjectDataLoader;
	gameObjectDataLoader.WriteSampleGameObjects();
	gameObjectDataLoader.LoadGameObjectInfo(&_data);

	_objectHandler = new ObjectHandler(_renderModule->GetDevice(), _assetManager, &_data, settings);
	_pickingDevice = new PickingDevice(_camera, settings);
	_SM = new StateMachine(_controls, _objectHandler, _camera, _pickingDevice, "Assets/gui.json", _assetManager, _fontWrapper, settings, &_settingsReader, &_soundModule);

	_SM->Update(_timer.GetFrameTime());

	_enemiesHasSpawned = false;
	_soundModule.AddSound("Assets/Sounds/theme", 0.15f, 1.0f, true, true);
	_soundModule.Play("Assets/Sounds/theme");
	
	ResizeResources(settings);//This fixes a bug which offsets mousepicking, do not touch! //Markus
}

Game::~Game()
{
	SAFE_DELETE(_objectHandler);
	SAFE_DELETE(_window);
	SAFE_DELETE(_renderModule);
	SAFE_DELETE(_camera);
	SAFE_DELETE(_SM);
	SAFE_DELETE(_controls);
	SAFE_DELETE(_assetManager);
	SAFE_DELETE(_pickingDevice);
	SAFE_DELETE(_fontWrapper);
	SAFE_DELETE(_particleHandler);
}

void Game::ResizeResources(System::Settings* settings)
{
	_window->ResizeWindow(settings);
	_SM->Resize(settings);
	_camera->Resize(settings);
	_renderModule->ResizeResources(settings);
}

bool Game::Update(double deltaTime)
{
	if (_SM->GetState() == PLACEMENTSTATE)
	{
		_objectHandler->UpdateLights();
	}
	_soundModule.Update();

	bool run = true;

	//Apply settings if they has changed
	if (_settingsReader.GetSettingsChanged())
	{
		ResizeResources(_settingsReader.GetSettings());
		_settingsReader.SetSettingsChanged(false);
	}

	_controls->Update();
	run = _SM->Update(deltaTime);




	//if (_controls->IsFunctionKeyDown("DEBUG:REQUEST_PARTICLE"))
	//{
	//	XMFLOAT3 pos = XMFLOAT3(rand() % 20 + 1, 0.0f, rand() % 20 + 1);
	//	XMFLOAT4 col = XMFLOAT4((rand() % 10) / 10.0f, (rand() % 10) / 10.0f, (rand() % 10) / 10.0f, 1.0f);

	//	ParticleRequestMessage msg = ParticleRequestMessage(ParticleType::SPLASH, pos, col, 10000.0f, 50, true);
	//	_particleHandler->GetParticleRequestQueue()->Insert(msg);
	//}
	//int witefoijesdjgsoieg = 34874685;
	//_particleHandler->Update(deltaTime);
	//int ewersdgfsdgsdssdg = 435634;



	if (_SM->GetState() == PLAYSTATE)
	{
		_enemies = _objectHandler->GetAllByType(ENEMY);
		_loot = _objectHandler->GetAllByType(LOOT);

		if (_enemies.size() > 0)
		{
			_enemiesHasSpawned = true;
		}
	}
	return run;
}

void Game::Render()
{
	_renderModule->BeginScene(0.0f, 0.5f, 0.5f, 1.0f);
	_renderModule->SetDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

	/*///////////////////////////////////////////////////////  Geometry pass  ////////////////////////////////////////////////////////////
	Render the objects to the diffuse and normal resource views. Camera depth is also generated here.									*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::GEO_PASS);

	std::vector<std::vector<GameObject*>>* gameObjects = _objectHandler->GetGameObjects();

	/*------------------------------------------------  Render non-skinned objects  ---------------------------------------------------*/
	for (auto i : *gameObjects)
	{
		if (i.size() > 0)
		{
			RenderObject* renderObject = i.at(0)->GetRenderObject();
			if (renderObject->_isSkinned)
			{
				continue;
			}
			else
			{
				_renderModule->SetDataPerObjectType(renderObject);
				int vertexBufferSize = renderObject->_mesh._vertexBufferSize;
				if (i.at(0)->IsVisible() || gameObjects->at(6).size() == 0)
				{
					_renderModule->Render(i.at(0)->GetMatrix(), vertexBufferSize, i.at(0)->GetColorOffset());
				}

				for (int j = 1; j < i.size(); j++)
				{
					if (i.at(j)->GetSubType() != i.at(j - 1)->GetSubType())
					{
						renderObject = i.at(j)->GetRenderObject();
						_renderModule->SetDataPerObjectType(renderObject);
						vertexBufferSize = renderObject->_mesh._vertexBufferSize;
					}
					if (i.at(j)->IsVisible() || gameObjects->at(6).size() == 0)
					{
						_renderModule->Render(i.at(j)->GetMatrix(), vertexBufferSize, i.at(j)->GetColorOffset());
					}
					
				}
			}
		}
	}

	/*--------------------------------------------------  Render skinned objects  -----------------------------------------------------*/
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_STAGE);
	// Now every gameobject can be animated
	for (auto i : *gameObjects)
	{
		if (i.size() > 0)
		{
			RenderObject* renderObject = i.at(0)->GetRenderObject();
			if (!renderObject->_isSkinned)
			{
				continue;
			}
			else
			{
				_renderModule->SetDataPerObjectType(renderObject);
				int vertexBufferSize = renderObject->_mesh._vertexBufferSize;
				if (i.at(0)->IsVisible() || gameObjects->at(6).size() == 0)
				{
					_renderModule->RenderAnimation(i.at(0)->GetMatrix(), vertexBufferSize, i.at(0)->GetAnimation()->GetFloats(), i.at(0)->GetColorOffset());
				}

				for (int j = 1; j < i.size(); j++)
				{
					if (i.at(j)->GetSubType() != i.at(j - 1)->GetSubType())
					{
						renderObject = i.at(j)->GetRenderObject();
						_renderModule->SetDataPerObjectType(renderObject);
						vertexBufferSize = renderObject->_mesh._vertexBufferSize;
					}
					if (i.at(j)->IsVisible() || gameObjects->at(6).size() == 0)
					{
						_renderModule->RenderAnimation(i.at(j)->GetMatrix(), vertexBufferSize, i.at(j)->GetAnimation()->GetFloats(), i.at(j)->GetColorOffset());
					}

				}
			}
		}
	}

	/*------------------------------------------------  Render billboarded objects  ---------------------------------------------------*/

	//_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::BILLBOARDING_STAGE);

	//int count = _particleHandler->GetEmitterCount();
	//for (int i = 0; i < count; i++)
	//{
	//    Renderer::ParticleEmitter* emitter = _particleHandler->GetEmitter(i);

	//	if (emitter)
	//	{
	//		ID3D11Buffer* vertexBuffer = emitter->GetParticleBuffer();

	//		if (vertexBuffer)
	//		{
	//			XMFLOAT3 color(0.1f, 0.6f, 0.25f);
	//			XMFLOAT3 pos(12, 1, 12);
	//			_renderModule->RenderParticles(vertexBuffer, emitter->GetBufferSize(), emitter->GetParticleCount(), pos, color);

	//			//_renderModule->RenderParticles(vertexBuffer, emitter->GetBufferSize(), emitter->GetParticleCount(), emitter->GetPosition(), color, emitter->GetTextures(), emitter->GetTextureCount());
	//		}
	//	}
	//}

	/*-------------------------------------------------------  Render grid  -----------------------------------------------------------*/
	if (_SM->GetState() == LEVELEDITSTATE)
	{
		Grid* gr = _objectHandler->GetBuildingGrid();

		_renderModule->SetShaderStage(Renderer::RenderModule::GRID_STAGE);
		_renderModule->SetDataPerLineList(gr->GetLineBuffer(), gr->GetVertexSize());

		std::vector<DirectX::XMMATRIX>* gridMatrices = gr->GetGridMatrices();
		for (auto &matrix : *gridMatrices)
		{
			_renderModule->RenderLineList(&matrix, gr->GetNrOfPoints(), gr->GetColorOffset());
		}
	}
	
	////////////////////////////////////////////////////////////  Light pass  //////////////////////////////////////////////////////////////
	if (_SM->GetState() == PLAYSTATE || _SM->GetState() == PLACEMENTSTATE)
	{
		//----------------------------------------------------------  Spotlights  -------------------------------------------------------------
		//Generate the shadow map for each spotlight, then apply the lighting/shadowing to the render target with additive blending.           

		_renderModule->SetLightDataPerFrame(_camera->GetViewMatrix(), _camera->GetProjectionMatrix());

		map<GameObject*, Renderer::Spotlight*>* spotlights = _objectHandler->GetSpotlights();
		for (pair<GameObject*, Renderer::Spotlight*> spot : *spotlights)
		{
			if (spot.second != nullptr && spot.second->IsActive() && spot.first->IsActive())
			{
				// Non skinned
				_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::SHADOW_GENERATION);
				_renderModule->SetShadowMapDataPerSpotlight(spot.second->GetViewMatrix(), spot.second->GetProjectionMatrix());

				vector<vector<GameObject*>>* inLight = _objectHandler->GetObjectsInLight(spot.second);
				for (auto j : *inLight)
				{
					if (j.size() > 0)
					{
						RenderObject* renderObject = j.at(0)->GetRenderObject();
						if (!renderObject->_isSkinned)
						{
							_renderModule->SetShadowMapDataPerObjectType(renderObject);
							int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

							for (GameObject* g : j)
							{
								if (g->IsVisible() && g->GetID() != spot.first->GetID() &&
									(g->IsVisible() || gameObjects->at(6).size() == 0))
								{
									_renderModule->RenderShadowMap(g->GetMatrix(), vertexBufferSize);
								}
							}
						}
					}
				}

				// Skinned
				_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::ANIM_SHADOW_GENERATION);
				_renderModule->SetShadowMapDataPerSpotlight(spot.second->GetViewMatrix(), spot.second->GetProjectionMatrix());

				inLight = _objectHandler->GetObjectsInLight(spot.second);
				for (auto j : *inLight)
				{
					if (j.size() > 0)
					{
						RenderObject* renderObject = j.at(0)->GetRenderObject();
						if (renderObject->_isSkinned)
						{
							_renderModule->SetShadowMapDataPerObjectType(renderObject);
							int vertexBufferSize = renderObject->_mesh._vertexBufferSize;

							for (GameObject* g : j)
							{
								if (g->IsVisible() && g->GetID() != spot.first->GetID())
								{
									_renderModule->RenderShadowMap(g->GetMatrix(), vertexBufferSize);
								}
							}
						}
					}
				}

				_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_SPOTLIGHT);
				_renderModule->SetLightDataPerSpotlight(spot.second);

				_renderModule->RenderLightVolume(spot.second->GetVolumeBuffer(), spot.second->GetWorldMatrix(), spot.second->GetVertexCount(), spot.second->GetVertexSize());
			}
		}
		/*---------------------------------------------------------  Pointlights  ------------------------------------------------------------*/
		_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::LIGHT_APPLICATION_POINTLIGHT);

		map<GameObject*, Renderer::Pointlight*>* pointlights = _objectHandler->GetPointlights();
		for (pair<GameObject*, Renderer::Pointlight*> pointlight : *pointlights)
		{
			if (pointlight.second != nullptr && pointlight.second->IsActive() && pointlight.first->IsActive())
			{
				_renderModule->SetLightDataPerPointlight(pointlight.second);
				_renderModule->RenderLightVolume(pointlight.second->GetVolumeBuffer(), pointlight.second->GetWorldMatrix(), pointlight.second->GetVertexCount(), pointlight.second->GetVertexSize());
			}
		}
	}

	/*-----------------------------------------------------------  FXAA  -----------------------------------------------------------------*/

	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::AA_STAGE);
	_renderModule->RenderScreenQuad();

	/////////////////////////////////////////////////////////  HUD and other 2D   ////////////////////////////////////////////////////
	_renderModule->SetShaderStage(Renderer::RenderModule::ShaderStage::HUD_STAGE);
	_renderModule->Render(_SM->GetCurrentStatePointer()->GetUITree()->GetRootNode(), _fontWrapper);

	_renderModule->EndScene();
}

int Game::Run()
{
	bool run = true;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (run)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				run = false;
			}
		}
		else
		{
			_timer.Update();
			if (_timer.GetFrameTime() >= MS_PER_FRAME)
			{
				if (_hasFocus)
				{
					run = Update(_timer.GetFrameTime());

					if (run)
					{
						Render();

						string s = to_string(_timer.GetFrameTime()) + " " + to_string(_timer.GetFPS());
						SetWindowText(_window->GetHWND(), s.c_str());

						_timer.Reset();
					}
				}
			}
		}
	}

	return 0;
}

LRESULT CALLBACK Game::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_QUIT:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CHAR:
	{
		_gameHandle->_controls->HandleTextInput(wparam, lparam);
		break;
	}
	case WM_INPUT:
	{
		_gameHandle->_controls->HandleRawInput(lparam);
		break;
	}
	case WM_SETFOCUS:
	{
		_gameHandle->_hasFocus = true;
		break;
	}
	case WM_KILLFOCUS:
	{
		_gameHandle->_controls->ResetInputBuffers();
		_gameHandle->_hasFocus = false;
		break;
	}
	default:
	{
		return _gameHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}