#pragma once
#include "GameObject.h"
#include "Unit.h"
class Trap : public GameObject
{
private:
	bool _isActive;
public:
	Trap();
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject * renderObject);
	~Trap();

	void Activate(Unit* target);
	void Update(float deltaTime);
	void Release();
};

