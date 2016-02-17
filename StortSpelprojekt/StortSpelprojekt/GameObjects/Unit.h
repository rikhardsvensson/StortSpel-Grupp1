#pragma once
#include "GameObject.h"
#include "../Tilemap.h"
#include "AStar.h"
#include "../VisionCone.h"
#include <DirectXMath.h>
class Unit : public GameObject
{
public:
	const float MOVE_SPEED = 0.025f;				//Movement per frame
private:
	AI::AStar* _aStar;
	AI::Vec2D _goalTilePosition;
	AI::Vec2D* _path;
	float  _moveSpeed;

	int _visionRadius;
	VisionCone* _visionCone;
	//AI::Vec2D* _visibleTiles;
	//int _nrOfVisibleTiles;

	int _waiting;					//Temporarily counting frame. Should use a timer eventually

	int _health;
	bool _isMoving;	

	//void ScanOctant(int depth, int octant, double &startSlope, double endSlope);
	//double GetSlope(double x1, double y1, double x2, double y2, bool invert);
	//int GetVisDistance(int x1, int y1, int x2, int y2);
	void CalculatePath();
	void Rotate();

protected:
	AI::Vec2D _direction;
	const Tilemap* _tileMap;		//Pointer to the tileMap in objectHandler(?). Units should preferably have read-, but not write-access.
	int _goalPriority;				//Lower value means higher priority
	int _pathLength;
	int _trapInteractionTime;
	GameObject* _heldObject;


	bool _isFleeing;
	GameObject* _pursuer;
	GameObject* _objective;

	int GetApproxDistance(AI::Vec2D target)const;
	void Flee();
	bool _stop;

public:
	Unit();
	Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap);
	virtual ~Unit();
	int GetPathLength()const;
	AI::Vec2D GetGoal();
	AI::Vec2D GetDirection();
	void SetDirection(const AI::Vec2D direction);
	int GetHealth();
	GameObject* GetHeldObject()const;
	//void FindVisibleTiles();
	void CheckVisibleTiles();
	void CheckAllTiles();
	virtual void EvaluateTile(Type objective, AI::Vec2D tile) = 0;
	virtual void EvaluateTile(GameObject* obj) = 0;
	void SetGoal(AI::Vec2D goal);
	void SetGoal(GameObject* objective);
	void Move();
	void Update(float deltaTime);
	virtual void Release();
	virtual void act(GameObject* obj) = 0;									//context specific action on the unit's objective
	void Wait(int frames);
	void ClearObjective();
	void TakeDamage(int damage);
	void SetVisibility(bool visible);
	void UseTrap();
	int GetVisionRadius()const;

	enum Anim { IDLEANIM, WALKANIM, FIXTRAPANIM, FIGHTANIM, PICKUPOBJECTANIM, NR_OF_ANIM/*Has to be last*/ };
	void Animate(Anim anim);
	bool GetAnimisFinished();

};
