/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

#ifndef CBASE_H
#define CBASE_H
#pragma once

class CBaseEntity;

//
// EHANDLE. Safe way to point to CBaseEntities who may die between frames
//
class EHANDLE
{
private:
	edict_t *m_pent;
	int		m_serialnumber;
public:
	edict_t *Get();
	edict_t *Set(edict_t *pent);

	operator int();

	operator CBaseEntity *();

	CBaseEntity * operator = (CBaseEntity *pEntity);
	CBaseEntity * operator ->();
};

typedef struct hash_item_s
{
	entvars_t			*pev;
	struct hash_item_s	*next;
	struct hash_item_s	*lastHash;
	int					pevIndex;

} hash_item_t;

class CSave;
class CRestore;
class CBasePlayer;
class CBaseEntity;
class CBaseMonster;
class CBasePlayerItem;
class CSquadMonster;
class CCSEntity;

#undef CREATE_NAMED_ENTITY
#undef REMOVE_ENTITY

//
// Base Entity.  All entity types derive from this
//
class CBaseEntity
{
public:
	// initialization functions
	virtual void Spawn();
	virtual void Precache();
	virtual void Restart();
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps();
	virtual void Activate();

	// Setup the object->object collision box (pev->mins / pev->maxs is the object->world collision box)
	virtual void SetObjectCollisionBox();

	// Classify - returns the type of group (i.e, "houndeye", or "human military" so that monsters with different classnames
	// still realize that they are teammates. (overridden for monsters that form groups)
	virtual int Classify();

	virtual void DeathNotice(entvars_t *pevChild);
	virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual BOOL TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
	virtual BOOL TakeHealth(float flHealth, int bitsDamageType);
	virtual void Killed(entvars_t *pevAttacker, int iGib);
	virtual int BloodColor();
	virtual void TraceBleed(float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	virtual BOOL IsTriggered(CBaseEntity *pActivator);
	virtual CBaseMonster *MyMonsterPointer();
	virtual CSquadMonster *MySquadMonsterPointer();
	virtual int GetToggleState();
	virtual void AddPoints(int score, BOOL bAllowNegativeScore);
	virtual void AddPointsToTeam(int score, BOOL bAllowNegativeScore);
	virtual BOOL AddPlayerItem(CBasePlayerItem *pItem);
	virtual BOOL RemovePlayerItem(CBasePlayerItem *pItem);
	virtual int GiveAmmo(int iAmount, const char *szName, int iMax = -1);
	virtual float GetDelay();
	virtual int IsMoving();
	virtual void OverrideReset();
	virtual int DamageDecal(int bitsDamageType);

	// This is ONLY used by the node graph to test movement through a door
	virtual void SetToggleState(int state);

	virtual void StartSneaking();
	virtual void StopSneaking();

	virtual BOOL OnControls(entvars_t *onpev);
	virtual BOOL IsSneaking();
	virtual BOOL IsAlive();
	virtual BOOL IsBSPModel();
	virtual BOOL ReflectGauss();
	virtual BOOL HasTarget(string_t targetname);
	virtual BOOL IsInWorld();
	virtual BOOL IsPlayer();
	virtual BOOL IsNetClient();
	virtual const char *TeamID();
	virtual CBaseEntity *GetNextTarget();
	virtual void Think();
	virtual void Touch(CBaseEntity *pOther);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, int , float value = 0.0f);
	virtual void Blocked(CBaseEntity *pOther);
	virtual CBaseEntity *Respawn();

public:
	// virtual functions used by a few classes

	// used by monsters that are created by the MonsterMaker
	virtual void UpdateOwner();

	virtual BOOL FBecomeProne();

	virtual Vector Center();
	virtual Vector EyePosition();
	virtual Vector EarPosition();
	virtual Vector BodyTarget(const Vector &posSrc);

	virtual int Illumination();

	virtual BOOL FVisible(CBaseEntity *pEntity);
	virtual BOOL FVisible(const Vector &vecOrigin);

public:
	// Constructor. Set engine to use C/C++ callback functions
	// pointers to engine data
	entvars_t *pev;					// Don't need to save/restore this pointer, the engine resets it

	// path corners
	CBaseEntity *m_pGoalEnt;		// path corner we are heading towards
	CBaseEntity *m_pLink;			// used for temporary link-list operations.

	// fundamental callbacks
	void (CBaseEntity::*m_pfnThink)();
	void (CBaseEntity::*m_pfnTouch)(CBaseEntity *pOther);
	void (CBaseEntity::*m_pfnUse)(CBaseEntity *pActivator, CBaseEntity *pCaller, int useType, float value);
	void (CBaseEntity::*m_pfnBlocked)(CBaseEntity *pOther);

	// We use this variables to store each ammo count.
	// let's sacrifice this unused member, for its own needs in favor of m_pEntity
	int *current_ammo;
	float currentammo;
	int maxammo_buckshot;
	int ammo_buckshot;
	int maxammo_9mm;
	int ammo_9mm;
	int maxammo_556nato;
	int ammo_556nato;
	int maxammo_556natobox;
	int ammo_556natobox;
	int maxammo_762nato;
	int ammo_762nato;
	int maxammo_45acp;
	int ammo_45acp;
	int maxammo_50ae;
	int ammo_50ae;
	int maxammo_338mag;
	int ammo_338mag;
	int maxammo_57mm;
	int ammo_57mm;
	int maxammo_357sig;
	int ammo_357sig;

	// Special stuff for grenades and knife.
	float m_flStartThrow;
	float m_flReleaseThrow;
	int m_iSwing;

	// client has left the game
	bool has_disconnected;
};

//
// generic Delay entity.
//
class CBaseDelay : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	float m_flDelay;
	string_t m_iszKillTarget;
};

class CBaseAnimating : public CBaseDelay
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void HandleAnimEvent(void *pEvent);

	// animation needs
	float m_flFrameRate;		// computed FPS for current sequence
	float m_flGroundSpeed;		// computed linear movement rate for current sequence
	float m_flLastEventCheck;	// last time the event list was checked
	BOOL m_fSequenceFinished;	// flag set when StudioAdvanceFrame moves across a frame boundry
	BOOL m_fSequenceLoops;		// true if the sequence loops
};

//
// generic Toggle entity.
//
#define	SF_ITEM_USE_ONLY	256 //  ITEM_USE_ONLY = BUTTON_USE_ONLY = DOOR_USE_ONLY!!! 

class CBaseToggle : public CBaseAnimating
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int GetToggleState();
	virtual float GetDelay();

	int m_toggle_state;
	float m_flActivateFinished;	// like attack_finished, but for doors
	float m_flMoveDistance;		// how far a door should slide or rotate
	float m_flWait;
	float m_flLip;
	float m_flTWidth;			// for plats
	float m_flTLength;			// for plats

	Vector m_vecPosition1;
	Vector m_vecPosition2;
	Vector m_vecAngle1;
	Vector m_vecAngle2;

	int m_cTriggersLeft;		// trigger_counter only, # of activations remaining
	float m_flHeight;
	EHANDLE m_hActivator;
	void (CBaseToggle::*m_pfnCallWhenMoveDone)();

	Vector m_vecFinalDest;
	Vector m_vecFinalAngle;

	int m_bitsDamageInflict;	// DMG_ damage type that the door or tigger does

	string_t m_sMaster;			// If this button has a master switch, this is the targetname.
								// A master switch must be of the multisource type. If all
								// of the switches in the multisource have been triggered, then
								// the button will be allowed to operate. Otherwise, it will be
								// deactivated.
};

#endif // CBASE_H