/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#ifndef WEAPONDEFS_H
#define WEAPONDEFS_H
#pragma once

// the maximum amount of ammo each weapon's clip can hold
#define WEAPON_NO_CLIP               -1

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

enum EAmmoType
{
	AMMO_NONE,

	AMMO_338MAGNUM,
	AMMO_762NATO,
	AMMO_556NATOBOX,
	AMMO_556NATO,
	AMMO_BUCKSHOT,
	AMMO_45ACP,
	AMMO_57MM,
	AMMO_50AE,
	AMMO_357SIG,
	AMMO_9MM,
	AMMO_FLASHBANG,
	AMMO_HEGRENADE,
	AMMO_SMOKEGRENADE,
	AMMO_C4,

	AMMO_MAX_TYPES
};

enum EBulletType
{
	BULLET_NONE,

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
	BULLET_PLAYER_357SIG,

	BULLET_MAX_TYPES
};

enum EWeaponClassType
{
	WEAPONCLASS_NONE,

	WEAPONCLASS_KNIFE,
	WEAPONCLASS_PISTOL,
	WEAPONCLASS_GRENADE,
	WEAPONCLASS_SUBMACHINEGUN,
	WEAPONCLASS_SHOTGUN,
	WEAPONCLASS_MACHINEGUN,
	WEAPONCLASS_RIFLE,
	WEAPONCLASS_SNIPERRIFLE,

	WEAPONCLASS_MAX,
};

enum EClipSizeType
{
	MAX_CLIP_NONE = 0,

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

enum EWeightWeapon
{
	P228_WEIGHT = 5,
	GLOCK18_WEIGHT = 5,
	SCOUT_WEIGHT = 30,
	HEGRENADE_WEIGHT = 2,
	XM1014_WEIGHT = 20,
	C4_WEIGHT = 3,
	MAC10_WEIGHT = 25,
	AUG_WEIGHT = 25,
	SMOKEGRENADE_WEIGHT = 1,
	ELITE_WEIGHT = 5,
	FIVESEVEN_WEIGHT = 5,
	UMP45_WEIGHT = 25,
	SG550_WEIGHT = 20,
	GALIL_WEIGHT = 25,
	FAMAS_WEIGHT = 75,
	USP_WEIGHT = 5,
	AWP_WEIGHT = 30,
	MP5NAVY_WEIGHT = 25,
	M249_WEIGHT = 25,
	M3_WEIGHT = 20,
	M4A1_WEIGHT = 25,
	TMP_WEIGHT = 25,
	G3SG1_WEIGHT = 20,
	FLASHBANG_WEIGHT = 1,
	DEAGLE_WEIGHT = 7,
	SG552_WEIGHT = 25,
	AK47_WEIGHT = 25,
	P90_WEIGHT = 26,
	KNIFE_WEIGHT = 0,
};

enum EWeaponCostType
{
	PRICE_FREE = 0,

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

enum EAmmoCostType
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
	AMMO_FLASHBANG_PRICE = FLASHBANG_PRICE,
	AMMO_HEGRENADE_PRICE = HEGRENADE_PRICE,
	AMMO_SMOKEGRENADE_PRICE = SMOKEGRENADE_PRICE,
};

enum EWeaponPrimaryDamage
{
	P228_PRIMARY_DAMAGE = 32,
	GLOCK_PRIMARY_DAMAGE = 0,
	SCOUT_PRIMARY_DAMAGE = 75,
	HEGRENADE_PRIMARY_DAMAGE = 0,
	XM1014_PRIMARY_DAMAGE = 4,
	C4_PRIMARY_DAMAGE = 0,
	MAC10_PRIMARY_DAMAGE = 29,
	AUG_PRIMARY_DAMAGE = 32,
	SMOKEGRENADE_PRIMARY_DAMAGE = 0,
	ELITE_PRIMARY_DAMAGE = 36,
	FIVESEVEN_PRIMARY_DAMAGE = 14,
	UMP45_PRIMARY_DAMAGE = 30,
	SG550_PRIMARY_DAMAGE = 40,
	GALIL_PRIMARY_DAMAGE = 33,
	FAMAS_PRIMARY_DAMAGE = 33,
	USP_PRIMARY_DAMAGE = 34,
	GLOCK18_PRIMARY_DAMAGE = 20,
	AWP_PRIMARY_DAMAGE = 115,
	MP5N_PRIMARY_DAMAGE = 26,
	M249_PRIMARY_DAMAGE = 32,
	M3_PRIMARY_DAMAGE = 4,
	M4A1_PRIMARY_DAMAGE = 32,
	TMP_PRIMARY_DAMAGE = 20,
	G3SG1_PRIMARY_DAMAGE = 60,
	FLASHBANG_PRIMARY_DAMAGE = 0,
	DEAGLE_PRIMARY_DAMAGE = 54,
	SG552_PRIMARY_DAMAGE = 33,
	AK47_PRIMARY_DAMAGE = 36,
	KNIFE_PRIMARY_DAMAGE = 0,
	P90_PRIMARY_DAMAGE = 21,
};

enum EWeaponSecondaryDamage
{
	P228_SECONDARY_DAMAGE = 0,
	GLOCK_SECONDARY_DAMAGE = 0,
	SCOUT_SECONDARY_DAMAGE = 0,
	HEGRENADE_SECONDARY_DAMAGE = 0,
	XM1014_SECONDARY_DAMAGE = 0,
	C4_SECONDARY_DAMAGE = 0,
	MAC10_SECONDARY_DAMAGE = 0,
	AUG_SECONDARY_DAMAGE = 0,
	SMOKEGRENADE_SECONDARY_DAMAGE = 0,
	ELITE_SECONDARY_DAMAGE = 0,
	FIVESEVEN_SECONDARY_DAMAGE = 0,
	UMP45_SECONDARY_DAMAGE = 0,
	SG550_SECONDARY_DAMAGE = 0,
	GALIL_SECONDARY_DAMAGE = 0,
	FAMAS_SECONDARY_DAMAGE = 0,
	USP_SECONDARY_DAMAGE = 30,
	GLOCK18_SECONDARY_DAMAGE = 0,
	AWP_SECONDARY_DAMAGE = 0,
	MP5N_SECONDARY_DAMAGE = 0,
	M249_SECONDARY_DAMAGE = 0,
	M3_SECONDARY_DAMAGE = 0,
	M4A1_SECONDARY_DAMAGE = 33,
	TMP_SECONDARY_DAMAGE = 0,
	G3SG1_SECONDARY_DAMAGE = 0,
	FLASHBANG_SECONDARY_DAMAGE = 0,
	DEAGLE_SECONDARY_DAMAGE = 0,
	SG552_SECONDARY_DAMAGE = 0,
	AK47_SECONDARY_DAMAGE = 0,
	KNIFE_SECONDARY_DAMAGE = 0,
	P90_SECONDARY_DAMAGE = 0,
};

enum EWeaponMaxSpeed
{
	P228_MAX_SPEED = 250,
	GLOCK_MAX_SPEED = 0,
	SCOUT_MAX_SPEED = 260,
	HEGRENADE_MAX_SPEED = 250,
	XM1014_MAX_SPEED = 240,
	C4_MAX_SPEED = 250,
	MAC10_MAX_SPEED = 250,
	AUG_MAX_SPEED = 240,
	SMOKEGRENADE_MAX_SPEED = 250,
	ELITE_MAX_SPEED = 250,
	FIVESEVEN_MAX_SPEED = 250,
	UMP45_MAX_SPEED = 250,
	SG550_MAX_SPEED = 210,
	GALIL_MAX_SPEED = 240,
	FAMAS_MAX_SPEED = 240,
	USP_MAX_SPEED = 250,
	GLOCK18_MAX_SPEED = 250,
	AWP_MAX_SPEED = 210,
	MP5N_MAX_SPEED = 250,
	M249_MAX_SPEED = 220,
	M3_MAX_SPEED = 230,
	M4A1_MAX_SPEED = 230,
	TMP_MAX_SPEED = 250,
	G3SG1_MAX_SPEED = 210,
	FLASHBANG_MAX_SPEED = 250,
	DEAGLE_MAX_SPEED = 250,
	SG552_MAX_SPEED = 235,
	AK47_MAX_SPEED = 221,
	KNIFE_MAX_SPEED = 250,
	P90_MAX_SPEED = 245,
};

enum EWeaponGenericMaxDistance
{
	WEAPON_MAX_DISTANCE_NONE = 0,
	WEAPON_MAX_DISTANCE_SMALLEST = 3000,
	WEAPON_MAX_DISTANCE_SMALL = 3048,
	WEAPON_MAX_DISTANCE_MEDIUM = 4096,
	WEAPON_MAX_DISTANCE_BIG = 8192
};

enum EWeaponMaxDistance
{
	P228_MAX_DISTANCE = WEAPON_MAX_DISTANCE_MEDIUM,
	GLOCK_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	SCOUT_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	HEGRENADE_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	XM1014_MAX_DISTANCE = WEAPON_MAX_DISTANCE_SMALL,
	C4_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	MAC10_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	AUG_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	SMOKEGRENADE_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	ELITE_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	FIVESEVEN_MAX_DISTANCE = WEAPON_MAX_DISTANCE_MEDIUM,
	UMP45_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	SG550_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	GALIL_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	FAMAS_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	USP_MAX_DISTANCE = WEAPON_MAX_DISTANCE_MEDIUM,
	GLOCK18_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	AWP_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	MP5N_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	M249_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	M3_MAX_DISTANCE = WEAPON_MAX_DISTANCE_SMALLEST,
	M4A1_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	TMP_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	G3SG1_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	FLASHBANG_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	DEAGLE_MAX_DISTANCE = WEAPON_MAX_DISTANCE_MEDIUM,
	SG552_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	AK47_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
	KNIFE_MAX_DISTANCE = WEAPON_MAX_DISTANCE_NONE,
	P90_MAX_DISTANCE = WEAPON_MAX_DISTANCE_BIG,
};

enum EWallPenetrationStrength
{
	WALL_PENETRATION_NONE,
	WALL_PENETRATION_1,
	WALL_PENETRATION_2,
	WALL_PENETRATION_3,
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
enum EWeaponAccuracyFlags
{
	WPNACCFLAG_NONE = 0,

	WPNACCFLAG_AIR = BIT(0), // accuracy depends on FL_ONGROUND
	WPNACCFLAG_SPEED = BIT(1),
	WPNACCFLAG_DUCK = BIT(2), // more accurate when ducking
	WPNACCFLAG_MULTIPLY_BY_14 = BIT(3), // accuracy multiply to 1.4
	WPNACCFLAG_MULTIPLY_BY_14_2 = BIT(4)
};

// weapon state flags
enum EWeaponState
{
	WPNSTATE_NONE = 0,

	WPNSTATE_USP_SILENCED = BIT(0),
	WPNSTATE_GLOCK18_BURST_MODE = BIT(1),
	WPNSTATE_M4A1_SILENCED = BIT(2),
	WPNSTATE_ELITE_LEFT = BIT(3),
	WPNSTATE_FAMAS_BURST_MODE = BIT(4),
	WPNSTATE_SHIELD_DRAWN = BIT(5),
};

#endif // WEAPONDEFS_H