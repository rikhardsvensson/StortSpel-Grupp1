#include "Unit.h"

void Unit::CalculatePath()
{
	if (_aStar->FindPath())
	{
		_path = _aStar->GetPath();
		_pathLength = _aStar->GetPathLength();
		//for (int i = 0; i < _pathLength; i++)
		//{
		//	/*if (_tileMap->IsFloorOnTile(_path[i]))
		//	{
		//		_tileMap->GetObjectOnTile(_path[i], FLOOR)->SetColorOffset({0,4,0});
		//	}*/
		//}
	}
	else
	{
		ClearObjective();
	}
	_moveState = MoveState::MOVING;
}

void Unit::Rotate()
{
	if (_direction._x != 0 || _direction._y != 0)
	{
		if (_direction._x == 0)
		{
			_rotation.y = DirectX::XM_PIDIV2 * (_direction._y + 1);
		}
		else if (_direction._x == -1)
		{
			_rotation.y = DirectX::XM_PIDIV2 + DirectX::XM_PIDIV4 * _direction._y;
		}
		else
		{
			_rotation.y = 3 * DirectX::XM_PIDIV2 - DirectX::XM_PIDIV4 * _direction._y;
		}
		CalculateMatrix();
	}
	//_visionCone->ColorVisibleTiles({0,0,0});
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
//	_visionCone->ColorVisibleTiles({0,0,3});
}

int Unit::GetApproxDistance(AI::Vec2D target) const
{
	return (int)_aStar->GetHeuristicDistance(_tilePosition, target);
}


/*
	Decides the best direction to run away from a foe.
*/
//void Unit::Flee()
//{
//	if (_pursuer == nullptr || !_visible)		//TODO Add other conditions to stop fleeing --Victor
//	{
//		CheckAllTiles();
//	//	Wait(20);
//	}
//	else
//	{
//		AI::Vec2D offset = _tilePosition - _pursuer->GetTilePosition();
//		AI::Vec2D bestDir = {0,0};
//		float bestDist = 0;
//		float tempDist = 0;
//		for (int i = 0; i < 8; i++)
//		{
//		//	AI::Vec2D tempDist = offset + AI::NEIGHBOUR_OFFSETS[i];
//
//			if (i < 4)						//weighting to normalize diagonal and straight directions
//			{
//				tempDist = pow(offset._x + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._x, 2) + pow(offset._y + AI::SQRT2 * AI::NEIGHBOUR_OFFSETS[i]._y, 2);
//			}
//			else
//			{
//				tempDist = pow(offset._x + AI::NEIGHBOUR_OFFSETS[i]._x, 2) + pow(offset._y + AI::NEIGHBOUR_OFFSETS[i]._y, 2);
//			}
//
//			if (_tileMap->IsFloorOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && !_tileMap->IsEnemyOnTile(_tilePosition + AI::NEIGHBOUR_OFFSETS[i]) && tempDist > bestDist)
//			{
//				bestDist = tempDist;
//				bestDir = AI::NEIGHBOUR_OFFSETS[i];
//			}
//		}
//		_direction = bestDir;
//		Rotate();
//	}
//}

Unit::Unit()
	: GameObject()
{
	_goalPriority = -1;
	_aStar = new AI::AStar();
	_visionCone = nullptr;
	_visionRadius = 0;
	_goalTilePosition = { 0,0 };
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;
	_pathLength = 0;
	_path = nullptr;
	_direction = {0, -1};
	_nextTile = _tilePosition;
	_interactionTime = -1;
	_isSwitchingTile = false;
	Rotate();
}

Unit::Unit(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, Type type, RenderObject* renderObject, const Tilemap* tileMap)
	: GameObject(ID, position, rotation, tilePosition, type, renderObject)
{
	_goalPriority = -1;
	_visionRadius = 6;
	_goalTilePosition = _tilePosition;
	_tileMap = tileMap;
	_visionCone = new VisionCone(_visionRadius, _tileMap);
	_aStar = new AI::AStar(_tileMap->GetWidth(), _tileMap->GetHeight(), _tilePosition, { 0,0 }, AI::AStar::OCTILE);
	_heldObject = nullptr;
	_objective = nullptr;
	_waiting = -1;
	_health = 1;					//TODO: Update constructor parameters to include health  --Victor
	_pathLength = 0;
	_path = nullptr;
	_direction = {0, 1};
	_nextTile = _tilePosition;
	_isSwitchingTile = false;
	Rotate();
	if (_renderObject->_isSkinned)
	{
		_animation = new Animation(_renderObject->_skeleton, true);
		_animation->Freeze(false);
	}
	_moveState = MoveState::IDLE;
	_interactionTime = -1;
}

Unit::~Unit()
{
	//delete[] _visibleTiles;
	delete _aStar;
	_aStar = nullptr;
	delete _visionCone;
	//delete _heldObject;
	//_heldObject = nullptr;
	if (_animation != nullptr)
	{
		delete _animation;
	}
}

int Unit::GetPathLength() const
{
	return _pathLength;
}

AI::Vec2D Unit::GetGoalTilePosition()
{
	return _goalTilePosition;
}

void Unit::SetGoalTilePosition(AI::Vec2D goal)
{
	if (_moveState != MoveState::FINDING_PATH)
	{
		ClearObjective();
		_goalTilePosition = goal;
		_moveState = MoveState::FINDING_PATH;
		//SetGoal(goal);
	}
}

AI::Vec2D Unit::GetDirection()
{
	return _direction;
}

void Unit::SetDirection(const AI::Vec2D direction)
{
	_direction = direction;
	_visionCone->FindVisibleTiles(_tilePosition, _direction);
}

int Unit::GetHealth()
{
	return _health;
}

GameObject * Unit::GetHeldObject() const
{
	return _heldObject;
}

Unit::MoveState Unit::GetMoveState() const
{
	return _moveState;
}


bool Unit::IsSwitchingTile() const
{
	return _isSwitchingTile;
}

void Unit::SetSwitchingTile(const bool switchTile)
{
	_isSwitchingTile = switchTile;
}

/*
Checks tiles that are visible to the unit
*/
void Unit::CheckVisibleTiles()
{
	AI::Vec2D* visibleTiles = _visionCone->GetVisibleTiles();
	for (int i = 0; i < _visionCone->GetNrOfVisibleTiles(); i++)
	{
		//if (_tileMap->IsWallOnTile(_visibleTiles[i]._x, _visibleTiles[i]._y))
		//{
		//	_aStar->SetTileCost(_visibleTiles[i], -1);
		//}
		if (_tileMap->IsTrapOnTile(visibleTiles[i]._x, visibleTiles[i]._y))											//TODO: Traps shouldn't be automatically visible --Victor
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], TRAP));
		}

		if (_type != ENEMY && _tileMap->IsEnemyOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], ENEMY));
		}
		if (_type != GUARD && _tileMap->IsGuardOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], GUARD));
		}
		if (_tileMap->IsObjectiveOnTile(visibleTiles[i]))
		{
			EvaluateTile(_tileMap->GetObjectOnTile(visibleTiles[i], LOOT));
		}
	}
}

void Unit::CheckAllTiles()
{
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle walls
			if (_tileMap->IsWallOnTile(AI::Vec2D(i, j)) || _tileMap->IsFurnitureOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({ i, j }, -1);
			}
			else
			{
				_aStar->SetTileCost({ i, j }, 1);
			}
		}
	}
	for (int i = 0; i < _tileMap->GetWidth(); i++)
	{
		for (int j = 0; j < _tileMap->GetHeight(); j++)
		{
			//Handle objectives
			if (_tileMap->IsObjectiveOnTile(AI::Vec2D(i, j)))
			{
				_aStar->SetTileCost({ i, j }, 1);
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), LOOT));
			}
			else if (_tileMap->IsTypeOnTile(AI::Vec2D(i, j), SPAWN))
			{
				EvaluateTile(_tileMap->GetObjectOnTile(AI::Vec2D(i, j), SPAWN));
			}
		}
	}
	//_aStar->printMap();
}

/*
Moves the goal and finds the path to the new goal
*/
void Unit::SetGoal(AI::Vec2D goal)
{
	if (_tileMap->IsTrapOnTile(goal))
	{
		SetGoal(_tileMap->GetObjectOnTile(goal, TRAP));
	}
 	else if (_tileMap->IsFloorOnTile(goal))
	{
		SetGoal(_tileMap->GetObjectOnTile(goal, FLOOR));
	}
	else
	{
		_moveState = MoveState::MOVING;
	}
}

void Unit::SetGoal(GameObject * objective)
{

	_goalTilePosition = objective->GetTilePosition();
	_objective = objective;
	//if (_objective->InRange(_tilePosition))
	//{
	//	//Act(_objective);
	//	_moveState = MoveState::AT_OBJECTIVE;
	//}
	//else
	//{
	_aStar->CleanMap();
	_aStar->SetStartPosition(_nextTile);
	_aStar->SetGoalPosition(_goalTilePosition);
	CalculatePath();

	//}
}

void Unit::Update(float deltaTime)
{
}

void Unit::Release()
{}

void Unit::Wait()
{
}

void Unit::ClearObjective()
{
	_objective = nullptr;
	_path = nullptr;
	_pathLength = 0;
}

void Unit::TakeDamage(int damage)
{
	_health -= damage;
}

void Unit::SetVisibility(bool visible)
{
	GameObject::SetVisibility(visible);
	if (_heldObject != nullptr)
	{
		_heldObject->SetVisibility(visible);
	}
}

void Unit::SetTilePosition(AI::Vec2D pos)
{
	GameObject::SetTilePosition(pos);
	if (_moveState == MoveState::IDLE)
	{
		_nextTile = pos;
	}
}

void Unit::Moving()
{
	if (IsCenteredOnTile(_nextTile))
	{
		_moveState = MoveState::SWITCHING_NODE;
		_isSwitchingTile = true;
		_position.x = _nextTile._x;
		_position.z = _nextTile._y;
	}
	else
	{
		if (_direction._x == 0 || _direction._y == 0)		//Right angle movement
		{
			_position.x += MOVE_SPEED * _direction._x;
			_position.z += MOVE_SPEED * _direction._y;
		}
		else												//Diagonal movement
		{
			_position.x += AI::SQRT2 * 0.5f * MOVE_SPEED * _direction._x;
			_position.z += AI::SQRT2 * 0.5f *MOVE_SPEED * _direction._y;
		}
		CalculateMatrix();
	}
}

void Unit::SwitchingNode()
{
	//_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,0,0});
	_tilePosition = _nextTile;
	//_tileMap->GetObjectOnTile(_tilePosition, FLOOR)->SetColorOffset({0,4,0});
	if (_objective != nullptr && _objective->GetPickUpState() == ONTILE)
	{
		if (_objective->InRange(_tilePosition))
		{
			_moveState = MoveState::AT_OBJECTIVE;
		}
		else if (_pathLength > 0 /*&& !_tileMap->IsGuardOnTile(_path[_pathLength - 1])*/)
		{
			_nextTile = _path[--_pathLength];
			_direction = _nextTile - _tilePosition;
			Rotate();
			_moveState = MoveState::MOVING;
			Animate(WALKANIM);
		}
		else			// TODO: else find unblocked path to goal --Victor
		{
			ClearObjective();
			_moveState = MoveState::IDLE;
		}
		_isSwitchingTile = false;
		CheckVisibleTiles();
	}
	else
	{
		ClearObjective();
		_moveState = MoveState::IDLE;
	}
}


/*
	Time is set to -1 when not active
	It gets set to 60 (temporary) when a disarm/repair attempt begins
	It ticks down one per frame until 0 at which the action resumes.
*/
void Unit::UseCountdown(int frames)
{

	if (_interactionTime < 0)
	{
		_interactionTime = frames;
		//Start
	}
	else if (_interactionTime > 0)
	{
		_interactionTime--;
	}
	else
	{
		//Finish
		Act(_objective);
		_interactionTime--;
	}
}

int Unit::GetVisionRadius() const
{
	return _visionRadius;
}

bool Unit::GetAnimisFinished()
{
	return _animation->GetisFinished();
}

void Unit::Animate(Anim anim)
{
	if (_renderObject->_isSkinned)
	{
		if (_renderObject->_type == GUARD)
		{
			switch (anim)
			{
			case IDLEANIM:
				_animation->SetActionAsCycle(0, 1.0f, false);
				break;
			case WALKANIM:
				_animation->SetActionAsCycle(1, 1.0f, false);
				break;
			case FIXTRAPANIM:
				_animation->SetActionAsCycle(2, 1.0f, false);
				break;
			case FIGHTANIM:
				_animation->PlayAction(4, 3.5f, false, false);
				break;
			default:
				break;
			}
		}
		if (_renderObject->_type == ENEMY)
		{
			switch (anim)
			{
			case IDLEANIM:
				_animation->SetActionAsCycle(0, 1.0f, false);
				break;
			case WALKANIM:
				_animation->SetActionAsCycle(1, 1.0f, false);
				break;
			case FIGHTANIM:
				_animation->PlayAction(1, 1.0f, false, false);
				break;
			case PICKUPOBJECTANIM:
				_animation->PlayAction(3, 1.0f, false, false);
				break;
			default:
				break;
			}
		}
	}
}
