#pragma once

#include "Unit.h"
#include "Enemy.h"

enum GuardType{BASICGUARD, ENGINEER, MARKSMAN};
class Guard : public Unit
{
private:
	bool _isSelected;
	std::vector<AI::Vec2D> _patrolRoute;
	unsigned int _currentPatrolGoal;
public:
	Guard(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue, const Tilemap* tileMap, const int GuardType = 0, AI::Vec2D direction = { 1, 0 });
	virtual ~Guard();
	void EvaluateTile(System::Type objective, AI::Vec2D tile);
	void EvaluateTile(GameObject* obj);
	void SetPatrolPoint(AI::Vec2D patrolPoint);
	std::vector<AI::Vec2D> GetPatrolRoute();
	void RemovePatrol();

	void Wait();
	void Release();

	void Update(float deltaTime);
	//void Moving();
	//void SetNextTile();			//replacement for move'
	void Act(GameObject* obj);
	void SwitchingNode();

	void ShowSelectIcon();
	void ShowPatrolIcons();
	void HideSelectIcon();
	void HidePatrolIcons();
};