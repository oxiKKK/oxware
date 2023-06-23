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
#ifndef WEAPONS_H
#define WEAPONS_H
#pragma once

#include "cbase.h"

class CBasePlayer;

enum ArmouryItemPack
{
	ARMOURY_MP5NAVY,
	ARMOURY_TMP,
	ARMOURY_P90,
	ARMOURY_MAC10,
	ARMOURY_AK47,
	ARMOURY_SG552,
	ARMOURY_M4A1,
	ARMOURY_AUG,
	ARMOURY_SCOUT,
	ARMOURY_G3SG1,
	ARMOURY_AWP,
	ARMOURY_M3,
	ARMOURY_XM1014,
	ARMOURY_M249,
	ARMOURY_FLASHBANG,
	ARMOURY_HEGRENADE,
	ARMOURY_KEVLAR,
	ARMOURY_ASSAULT,
	ARMOURY_SMOKEGRENADE,
	ARMOURY_SHIELD,
	ARMOURY_FAMAS,
	ARMOURY_SG550,
	ARMOURY_GALIL,
	ARMOURY_UMP45,
	ARMOURY_GLOCK18,
	ARMOURY_USP,
	ARMOURY_ELITE,
	ARMOURY_FIVESEVEN,
	ARMOURY_P228,
	ARMOURY_DEAGLE,
};

struct MULTIDAMAGE
{
	CBaseEntity *pEntity;
	float amount;
	int type;
};

class CArmoury : public CBaseEntity
{
public:
	virtual void Spawn();
	virtual void Precache();
	virtual void Restart();
	virtual void KeyValue(KeyValueData *pkvd);

	ArmouryItemPack m_iItem;
	int m_iCount;
	int m_iInitialCount;
	bool m_bAlreadyCounted;
};

class ItemInfo;
// Items that the player has in their inventory that they can use
class CCSPlayerItem;
class CBasePlayerItem : public CBaseAnimating
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void SetObjectCollisionBox();
	virtual CBaseEntity *Respawn();
	virtual int AddToPlayer(CBasePlayer *pPlayer);							// return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate(CBasePlayerItem *pItem);		// return TRUE if you want your duplicate removed from world
	virtual int GetItemInfo(ItemInfo *p);						// returns 0 if struct not filled out
	virtual BOOL CanDeploy();
	virtual BOOL CanDrop();								// returns is deploy was successful
	virtual BOOL Deploy();
	virtual BOOL IsWeapon();
	virtual BOOL CanHolster();							// can this weapon be put away right now?
	virtual void Holster(int skiplocal = 0);
	virtual void UpdateItemInfo();
	virtual void ItemPreFrame();											// called each frame by the player PreThink
	virtual void ItemPostFrame();										// called each frame by the player PostThink
	virtual void Drop();
	virtual void Kill();
	virtual void AttachToPlayer(CBasePlayer *pPlayer);
	virtual int PrimaryAmmoIndex();
	virtual int SecondaryAmmoIndex();
	virtual int UpdateClientData(CBasePlayer *pPlayer);
	virtual CBasePlayerItem *GetWeaponPtr();
	virtual float GetMaxSpeed();

public:
	virtual int iItemSlot() { return 0; }									// return 0 to MAX_ITEMS_SLOTS, used in hud

public:

	CBasePlayer *m_pPlayer;
	CBasePlayerItem *m_pNext;
	int m_iId;							// WEAPON_???
};

// inventory items that
class CCSPlayerWeapon;
class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer(CBasePlayer *pPlayer);
	virtual int AddDuplicate(CBasePlayerItem *pItem);
	virtual BOOL CanDeploy();
	virtual BOOL IsWeapon();
	virtual void Holster(int skiplocal = 0);
	virtual void UpdateItemInfo();
	virtual void ItemPostFrame();
	virtual int PrimaryAmmoIndex();
	virtual int SecondaryAmmoIndex();
	virtual int UpdateClientData(CBasePlayer *pPlayer);
	virtual CBasePlayerItem *GetWeaponPtr();
	virtual int ExtractAmmo(CBasePlayerWeapon *pWeapon);
	virtual int ExtractClipAmmo(CBasePlayerWeapon *pWeapon);
	virtual int AddWeapon();
	virtual BOOL PlayEmptySound();
	virtual void ResetEmptySound();
	virtual void SendWeaponAnim(int iAnim, int skiplocal = 0);
	virtual BOOL IsUseable();
	virtual void PrimaryAttack();
	virtual void SecondaryAttack();
	virtual void Reload();
	virtual void WeaponIdle();
	virtual void RetireWeapon();
	virtual BOOL ShouldWeaponIdle();
	virtual BOOL UseDecrement();

	int m_iPlayEmptySound;
	int m_fFireOnEmpty;
	float m_flNextPrimaryAttack;	// soonest time ItemPostFrame will call PrimaryAttack
	float m_flNextSecondaryAttack;	// soonest time ItemPostFrame will call SecondaryAttack
	float m_flTimeWeaponIdle;		// soonest time ItemPostFrame will call WeaponIdle
	int m_iPrimaryAmmoType;			// "primary" ammo index into players m_rgAmmo[]
	int m_iSecondaryAmmoType;		// "secondary" ammo index into players m_rgAmmo[]
	int m_iClip;					// number of shots left in the primary weapon clip, -1 it not used
	int m_iClientClip;				// the last version of m_iClip sent to hud dll
	int m_iClientWeaponState;		// the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int m_fInReload;				// Are we in the middle of a reload;
	int m_fInSpecialReload;			// Are we in the middle of a reload for the shotguns
	int m_iDefaultAmmo;				// how much ammo you get when you pick up this weapon as placed by a level designer.
	int m_iShellId;
	float m_fMaxSpeed;
	bool m_bDelayFire;
	int m_iDirection;
	bool m_bSecondarySilencerOn;
	float m_flAccuracy;
	float m_flLastFire;
	int m_iShotsFired;
	Vector m_vVecAiming;
	string_t model_name;
	float m_flGlock18Shoot;			// time to shoot the remaining bullets of the glock18 burst fire
	int m_iGlock18ShotsFired;		// used to keep track of the shots fired during the Glock18 burst fire mode.
	float m_flFamasShoot;
	int m_iFamasShotsFired;
	float m_fBurstSpread;
	int m_iWeaponState;
	float m_flNextReload;
	float m_flDecreaseShotsFired;
	unsigned short m_usFireGlock18;
	unsigned short m_usFireFamas;

	// hle time creep vars
	float m_flPrevPrimaryAttack;
	float m_flLastFireTime;
};

#endif // WEAPONS_H
