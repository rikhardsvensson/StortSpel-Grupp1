#include "Architecture.h"

Architecture::Architecture()
{

}
Architecture::Architecture(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject* renderObject, System::SoundModule* soundModule, int subType)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject, soundModule, DirectX::XMFLOAT3(0, 0, 0), subType)
{

}
Architecture::~Architecture(){}

void Architecture::Update(float deltaTime)
{}

void Architecture::Release()
{}

bool Architecture::InRange(AI::Vec2D pos) const
{
	if (_type == System::LOOT)
	{
		return GameObject::InRange(pos);
	}
	else
	{
		return pos == _tilePosition;
	}
	
}
