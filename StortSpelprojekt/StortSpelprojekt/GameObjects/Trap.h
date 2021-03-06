#pragma once
#include "GameObject.h"
#include "Unit.h"
#include "ParticleSystem\ParticleEventQueue.h"
#include <memory>

enum TrapType{ ANVIL, TESLACOIL, SHARK, GUN, SAW, CAKEBOMB, BEAR, FLAMETHROWER, WATER_GUN, SPIN_TRAP};
enum Anim{IDLE, ACTIVATE, NR_OF_ANIM/*Has to be last*/};

class Trap : public GameObject
{
private:
	bool _isActive;
	bool _isVisibleToEnemies;
	int _damage;

	Unit::StatusEffect _statusEffect;
	int _statusTimer;					//Time the status lasts
	int _statusInterval;				//time between status effects. Set to same time as _statusTimer to only activate once.
	int _detectDifficulty;
	int _disarmDifficulty;

	int _triggerTimer;					//Used either for moving triggers around (e.g. spinning sawblade) or time between automatic reloads (e.g. firearms)
	int	_maxTimeToTrigger;				//What _triggerTime resets to
	int _maxAmmunition;
	int _currentAmmunition;				//Number of times the traps can trigger before needing a guard to reset it. -1 = infinite

	int _repairTime;					//Amount of frames needed before modifiers to repair the trap
	int _disarmTime;					//Amount of frames needed before modifiers to disarm the trap

	bool _resetAnimTime;				//Resets the animation time on play action

	const Tilemap* _tileMap;

	AI::Vec2D* _occupiedTiles;			//Physical area taken up by the trap
	int _nrOfOccupiedTiles;
	AI::Vec2D* _triggerTiles;			//Tiles that when walked on by an enemy, will activate the trap. Things will get weird if _tilePosition is not a trigger.
	int _nrOfTriggers;
	AI::Vec2D* _areaOfEffect;			//Tiles that will be affected by the activated trap
	int _nrOfAOETiles;
	int _areaOfEffectArrayCapacity;

	int _sawBladeDirection;				//Checks which way the sawblade is moving

	int CalculateCircle(int radius, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateLine(int length, AI::Vec2D basePosition, AI::Vec2D* arr);
	int CalculateRectangle(int length, int width, AI::Vec2D basePosition, AI::Vec2D* arr);
	bool IsUnblocked(AI::Vec2D pos);
	AI::Vec2D ConvertOctant(int octant, AI::Vec2D pos, bool in = true);

	void Initialize(int damage, int tileSize, int triggerSize, int AOESize, int detectDifficulty, int disarmDifficulty, int repairTime, int disarmTime,
					Unit::StatusEffect statusEffect, int statusTimer, int statusInterval, int triggerTimer = 0, int ammunition = 1);							/*Initialize adds any characteristics specific to the subtype*/
	void SetTiles();
public:
	Trap(unsigned short ID, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, AI::Vec2D tilePosition, System::Type type, RenderObject * renderObject, System::SoundModule* soundModule, Renderer::ParticleEventQueue* particleEventQueue,
		 const Tilemap* tileMap, int trapType = ANVIL, AI::Vec2D direction = {1,0});
	virtual ~Trap();

	AI::Vec2D* GetTiles()const;
	int GetNrOfOccupiedTiles()const;
	AI::Vec2D* GetTriggers()const;
	int GetNrOfTriggerTiles()const;
	AI::Vec2D* GetAOE()const;
	int GetNrOfAOETiles()const;
	int GetDetectionDifficulty()const;
	int GetDisarmDifficulty()const;
	bool InRange(AI::Vec2D pos) const;
	bool IsTrapActive() const;
	void SetTrapActive(bool active);

	void RequestParticleByType(Unit* unit);

	void Activate();
	void Update(float deltaTime);
	void Release();

	bool IsVisibleToEnemies()const;
	void SetVisibleToEnemies(bool visible);

	// Overloaded function
	void SetTilePosition(AI::Vec2D pos);
	AI::Vec2D GetDirection();
	void SetDirection(const AI::Vec2D& direction);
	enum Anim { IDLEANIM, ACTIVATEANIM, DISABLEANIM, FIXANIM, NR_OF_ANIM/*Has to be last*/ };
	
	void Animate(Anim anim);

	// Overloaded because of damage/special and multiple tile positions
	void ShowAreaOfEffect();

	//Sound
	void PlayActivateSound();
};

