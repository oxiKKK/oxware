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

//
// weapondefs.h -- Shared definitions&constants for weapons used in CS
//

#ifndef WEAPONDEFS_H
#define WEAPONDEFS_H
#pragma once

typedef enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM,
	BULLET_PLAYER_MP5,
	BULLET_PLAYER_357,
	BULLET_PLAYER_BUCKSHOT,
	BULLET_PLAYER_CROWBAR,

	BULLET_MONSTER_9MM,
	BULLET_MONSTER_MP5,
	BULLET_MONSTER_12MM,

	BULLET_PLAYER_45ACP,
	BULLET_PLAYER_338MAG,
	BULLET_PLAYER_762MM,
	BULLET_PLAYER_556MM,
	BULLET_PLAYER_50AE,
	BULLET_PLAYER_57MM,
	BULLET_PLAYER_357SIG
}
Bullet;

enum shieldgun_e
{
	SHIELDGUN_IDLE,
	SHIELDGUN_SHOOT1,
	SHIELDGUN_SHOOT2,
	SHIELDGUN_SHOOT_EMPTY,
	SHIELDGUN_RELOAD,
	SHIELDGUN_DRAW,
	SHIELDGUN_DRAWN_IDLE,
	SHIELDGUN_UP,
	SHIELDGUN_DOWN
};

enum shieldren_e
{
	SHIELDREN_IDLE = 4,
	SHIELDREN_UP,
	SHIELDREN_DOWN
};

enum WeaponIdType
{
	WEAPON_NONE = 0,
	WEAPON_P228 = 1,
	WEAPON_GLOCK = 2,
	WEAPON_SCOUT = 3,
	WEAPON_HEGRENADE = 4,
	WEAPON_XM1014 = 5,
	WEAPON_C4 = 6,
	WEAPON_MAC10 = 7,
	WEAPON_AUG = 8,
	WEAPON_SMOKEGRENADE = 9,
	WEAPON_ELITE = 10,
	WEAPON_FIVESEVEN = 11,
	WEAPON_UMP45 = 12,
	WEAPON_SG550 = 13,
	WEAPON_GALIL = 14,
	WEAPON_FAMAS = 15,
	WEAPON_USP = 16,
	WEAPON_GLOCK18 = 17,
	WEAPON_AWP = 18,
	WEAPON_MP5N = 19,
	WEAPON_M249 = 20,
	WEAPON_M3 = 21,
	WEAPON_M4A1 = 22,
	WEAPON_TMP = 23,
	WEAPON_G3SG1 = 24,
	WEAPON_FLASHBANG = 25,
	WEAPON_DEAGLE = 26,
	WEAPON_SG552 = 27,
	WEAPON_AK47 = 28,
	WEAPON_KNIFE = 29,
	WEAPON_P90 = 30,
	WEAPON_SHIELDGUN = 99,
};

enum AmmoCostType
{
	AMMO_338MAG_PRICE = 125,
	AMMO_357SIG_PRICE = 50,
	AMMO_45ACP_PRICE = 25,
	AMMO_50AE_PRICE = 40,
	AMMO_556MM_PRICE = 60,
	AMMO_57MM_PRICE = 50,
	AMMO_762MM_PRICE = 80,
	AMMO_9MM_PRICE = 20,
	AMMO_BUCKSHOT_PRICE = 65,
};

enum WeaponCostType
{
	AK47_PRICE = 2500,
	AWP_PRICE = 4750,
	DEAGLE_PRICE = 650,
	G3SG1_PRICE = 5000,
	SG550_PRICE = 4200,
	GLOCK18_PRICE = 400,
	M249_PRICE = 5750,
	M3_PRICE = 1700,
	M4A1_PRICE = 3100,
	AUG_PRICE = 3500,
	MP5NAVY_PRICE = 1500,
	P228_PRICE = 600,
	P90_PRICE = 2350,
	UMP45_PRICE = 1700,
	MAC10_PRICE = 1400,
	SCOUT_PRICE = 2750,
	SG552_PRICE = 3500,
	TMP_PRICE = 1250,
	USP_PRICE = 500,
	ELITE_PRICE = 800,
	FIVESEVEN_PRICE = 750,
	XM1014_PRICE = 3000,
	GALIL_PRICE = 2000,
	FAMAS_PRICE = 2250,
	SHIELDGUN_PRICE = 2200,
};

enum ItemCostType
{
	ASSAULTSUIT_PRICE = 1000,
	FLASHBANG_PRICE = 200,
	HEGRENADE_PRICE = 300,
	SMOKEGRENADE_PRICE = 300,
	KEVLAR_PRICE = 650,
	HELMET_PRICE = 350,
	NVG_PRICE = 1250,
	DEFUSEKIT_PRICE = 200,
};

enum ClipSizeType
{
	P228_MAX_CLIP = 13,
	GLOCK18_MAX_CLIP = 20,
	SCOUT_MAX_CLIP = 10,
	XM1014_MAX_CLIP = 7,
	MAC10_MAX_CLIP = 30,
	AUG_MAX_CLIP = 30,
	ELITE_MAX_CLIP = 30,
	FIVESEVEN_MAX_CLIP = 20,
	UMP45_MAX_CLIP = 25,
	SG550_MAX_CLIP = 30,
	GALIL_MAX_CLIP = 35,
	FAMAS_MAX_CLIP = 25,
	USP_MAX_CLIP = 12,
	AWP_MAX_CLIP = 10,
	MP5N_MAX_CLIP = 30,
	M249_MAX_CLIP = 100,
	M3_MAX_CLIP = 8,
	M4A1_MAX_CLIP = 30,
	TMP_MAX_CLIP = 30,
	G3SG1_MAX_CLIP = 20,
	DEAGLE_MAX_CLIP = 7,
	SG552_MAX_CLIP = 30,
	AK47_MAX_CLIP = 30,
	P90_MAX_CLIP = 50,
};

enum MaxAmmoType
{
	MAX_AMMO_BUCKSHOT = 32,
	MAX_AMMO_9MM = 120,
	MAX_AMMO_556NATO = 90,
	MAX_AMMO_556NATOBOX = 200,
	MAX_AMMO_762NATO = 90,
	MAX_AMMO_45ACP = 100,
	MAX_AMMO_50AE = 35,
	MAX_AMMO_338MAGNUM = 30,
	MAX_AMMO_57MM = 100,
	MAX_AMMO_357SIG = 52
};

enum AmmoType
{
	AMMO_BUCKSHOT = 0,
	AMMO_9MM = 1,
	AMMO_556NATO = 2,
	AMMO_556NATOBOX = 3,
	AMMO_762NATO = 4,
	AMMO_45ACP = 5,
	AMMO_50AE = 6,
	AMMO_338MAGNUM = 7,
	AMMO_57MM = 8,
	AMMO_357SIG = 9,
	AMMO_MAX_TYPES = 10,
};

// NOTE: Nonexistent in GS
enum ArmorType
{
	ARMOR_NONE,     // No armor
	ARMOR_KEVLAR,   // Body vest only
	ARMOR_VESTHELM, // Vest and helmet
};

struct RebuyStruct
{
	int			m_primaryWeapon;
	int			m_primaryAmmo;
	int			m_secondaryWeapon;
	int			m_secondaryAmmo;
	int			m_heGrenade;
	int			m_flashbang;
	int			m_smokeGrenade;
	int			m_defuser;
	int			m_nightVision;
	ArmorType	m_armor;
};

struct WeaponInfoStruct
{
	int			id;
	int			cost;
	int			clipCost;
	int			buyClipSize;
	int			gunClipSize;
	int			maxRounds;
	int			ammoType;
	const char* entityName;
};

enum WeaponClassType
{
	WEAPONCLASS_NONE = 0,
	WEAPONCLASS_KNIFE = 1,
	WEAPONCLASS_PISTOL = 2,
	WEAPONCLASS_GRENADE = 3,
	WEAPONCLASS_SUBMACHINEGUN = 4,
	WEAPONCLASS_SHOTGUN = 5,
	WEAPONCLASS_MACHINEGUN = 6,
	WEAPONCLASS_RIFLE = 7,
	WEAPONCLASS_SNIPERRIFLE = 8,
	WEAPONCLASS_MAX = 9,
};

enum AutoBuyClassType
{
	AUTOBUYCLASS_PRIMARY = 1,
	AUTOBUYCLASS_SECONDARY = 2,
	AUTOBUYCLASS_AMMO = 4,
	AUTOBUYCLASS_ARMOR = 8,
	AUTOBUYCLASS_DEFUSER = 16,
	AUTOBUYCLASS_PISTOL = 32,
	AUTOBUYCLASS_SMG = 64,
	AUTOBUYCLASS_RIFLE = 128,
	AUTOBUYCLASS_SNIPERRIFLE = 256,
	AUTOBUYCLASS_SHOTGUN = 512,
	AUTOBUYCLASS_MACHINEGUN = 1024,
	AUTOBUYCLASS_GRENADE = 2048,
	AUTOBUYCLASS_NIGHTVISION = 4096,
	AUTOBUYCLASS_SHIELD = 8192,
};

struct AutoBuyInfoStruct
{
	AutoBuyClassType m_class;
	char* m_command;
	char* m_classname;
};

#define ITEM_HEALTHKIT					1
#define ITEM_ANTIDOTE					2
#define ITEM_SECURITY					3
#define ITEM_BATTERY					4

#define WEAPON_ALLWEAPONS				(~(1 << WEAPON_SUIT))
#define WEAPON_SUIT						31
#define MAX_WEAPONS						32

#define MAX_WEAPON_SLOTS				5	// hud item selection slots
#define MAX_ITEM_TYPES					6	// hud item selection slots

#define MAX_ITEMS						5	// hard coded item types

#define MAX_NORMAL_BATTERY				100

#define WEAPON_NOCLIP					-1

#define AK47_DEFAULT_GIVE				30
#define AUG_DEFAULT_GIVE				30
#define AWP_DEFAULT_GIVE				10
#define C4_DEFAULT_GIVE					1
#define DEAGLE_DEFAULT_GIVE				7
#define ELITE_DEFAULT_GIVE				30
#define FAMAS_DEFAULT_GIVE				25
#define FIVESEVEN_DEFAULT_GIVE			20
#define FLASHBANG_DEFAULT_GIVE			1
#define G3SG1_DEFAULT_GIVE				20
#define GALIL_DEFAULT_GIVE				35
#define GLOCK18_DEFAULT_GIVE			20
#define HEGRENADE_DEFAULT_GIVE			1
#define M249_DEFAULT_GIVE				100
#define M3_DEFAULT_GIVE					8
#define M4A1_DEFAULT_GIVE				30
#define MAC10_DEFAULT_GIVE				30
#define MP5NAVY_DEFAULT_GIVE			30
#define P228_DEFAULT_GIVE				13
#define P90_DEFAULT_GIVE				50
#define SCOUT_DEFAULT_GIVE				10
#define SG550_DEFAULT_GIVE				30
#define SG552_DEFAULT_GIVE				30
#define MAC10_DEFAULT_GIVE				30
#define SMOKEGRENADE_DEFAULT_GIVE		1
#define TMP_DEFAULT_GIVE				30
#define UMP45_DEFAULT_GIVE				25
#define USP_DEFAULT_GIVE				12
#define XM1014_DEFAULT_GIVE				7

#define AMMO_9MM_GIVE					30
#define AMMO_BUCKSHOT_GIVE				8
#define AMMO_556NATO_GIVE				30
#define AMMO_556NATOBOX_GIVE			30
#define AMMO_762NATO_GIVE				30
#define AMMO_45ACP_GIVE					12
#define AMMO_50AE_GIVE					7
#define AMMO_338MAGNUM_GIVE				10
#define AMMO_57MM_GIVE					50
#define AMMO_357SIG_GIVE				13

#define _9MM_MAX_CARRY					MAX_AMMO_9MM
#define BUCKSHOT_MAX_CARRY				MAX_AMMO_BUCKSHOT
#define _556NATO_MAX_CARRY				MAX_AMMO_556NATO
#define _556NATOBOX_MAX_CARRY			MAX_AMMO_556NATOBOX
#define _762NATO_MAX_CARRY				MAX_AMMO_762NATO
#define _45ACP_MAX_CARRY				MAX_AMMO_45ACP
#define _50AE_MAX_CARRY					MAX_AMMO_50AE
#define _338MAGNUM_MAX_CARRY			MAX_AMMO_338MAGNUM
#define _57MM_MAX_CARRY					MAX_AMMO_57MM
#define _357SIG_MAX_CARRY				MAX_AMMO_357SIG

#define HEGRENADE_MAX_CARRY				1
#define FLASHBANG_MAX_CARRY				2
#define SMOKEGRENADE_MAX_CARRY			1
#define C4_MAX_CARRY					1

#define ITEM_FLAG_SELECTONEMPTY			0x00001
#define ITEM_FLAG_NOAUTORELOAD			0x00002
#define ITEM_FLAG_NOAUTOSWITCHEMPTY		0x00004
#define ITEM_FLAG_LIMITINWORLD			0x00008
#define ITEM_FLAG_EXHAUSTIBLE			0x00010 // A player can totally exhaust their ammo supply and lose this weapon
#define ITEM_FLAG_NOFIREUNDERWATER  	0x00020

#define WPNSLOT_PRIMARY					1 // rifles, snipers, etc
#define WPNSLOT_SECONDARY				2 // pistols
#define WPNSLOT_KNIFE					3 // melee
#define WPNSLOT_GRENADE					4 // throwables
#define WPNSLOT_C4						5 // bomb

#define WEAPON_IS_ONTARGET				0x40

typedef struct
{
	int				iSlot;
	int				iPosition;
	const char		*pszAmmo1;
	int				iMaxAmmo1;
	const char		*pszAmmo2;
	int				iMaxAmmo2;
	const char		*pszName;
	int				iMaxClip;
	int				iId;
	int				iFlags;
	int				iWeight;
}
ItemInfo;

typedef struct
{
	const char		*pszName;
	int				iId;
}
AmmoInfo;

#define LOUD_GUN_VOLUME					1000
#define NORMAL_GUN_VOLUME				600
#define QUIET_GUN_VOLUME				200

#define BRIGHT_GUN_FLASH				512
#define NORMAL_GUN_FLASH				256
#define DIM_GUN_FLASH					128

#define BIG_EXPLOSION_VOLUME			2048
#define NORMAL_EXPLOSION_VOLUME			1024
#define SMALL_EXPLOSION_VOLUME			512

#define WEAPON_ACTIVITY_VOLUME			64

// player data iuser3
#define PLAYER_CAN_SHOOT					(1<<0)
#define PLAYER_FREEZE_TIME_OVER				(1<<1)
#define PLAYER_IN_BOMB_ZONE					(1<<2)
#define PLAYER_HOLDING_SHIELD				(1<<3)
#define PLAYER_PREVENT_DUCK					(1<<4)
#define PLAYER_PREVENT_CLIMB				(1<<5) // The player can't climb ladder

// accuracy flags
#define ACCURACY_NONE						0
#define ACCURACY_AIR						(1 << 0) // accuracy depends on FL_ONGROUND
#define ACCURACY_SPEED						(1 << 1)
#define ACCURACY_DUCK						(1 << 2) // more accurate when ducking
#define ACCURACY_MULTIPLY_BY_14				(1 << 3) // accuracy multiply to 1.4
#define ACCURACY_MULTIPLY_BY_14_2			(1 << 4) // accuracy multiply to 1.4

#endif // WEAPONDEFS_H