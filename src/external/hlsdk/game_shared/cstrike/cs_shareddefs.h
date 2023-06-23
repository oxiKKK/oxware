//========= Copyright (c) 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Definitions that are shared by the game DLL and the client DLL.
//
// $NoKeywords: $
//=============================================================================

//
// cs_shareddefs.h -- shareddefs.h version for cstrike.
//

#ifndef CS_SHAREDDEFS_H
#define CS_SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

#define MAX_WEAPONS 32

// CS-specific menus
#define MENU_TERRORIST				26
#define MENU_CT						27
#define MENU_BUY					28
#define MENU_PISTOL					29
#define MENU_SHOTGUN				30
#define MENU_RIFLE					31
#define MENU_SMG					32
#define MENU_MACHINEGUN				33
#define MENU_EQUIPMENT				34
// career
#define MENU_CAREER_ROUNDEND		35
#define MENU_CAREER_MATCHEND		36
#define MENU_CAREER_CREDITSMENU		37
#define MENU_CAREER_BUYPRESETMAIN	38

typedef enum
{
	Menu_OFF,
	Menu_ChooseTeam,
	Menu_IGChooseTeam,
	Menu_ChooseAppearance,
	Menu_Buy,
	Menu_BuyPistol,
	Menu_BuyRifle,
	Menu_BuyMachineGun,
	Menu_BuyShotgun,
	Menu_BuySubMachineGun,
	Menu_BuyItem,
	Menu_Radio1,
	Menu_Radio2,
	Menu_Radio3,
	Menu_ClientBuy
} CSMenu; // collided with vgui2::Menu when file used 'using namespace vgui2'

typedef enum // NOTE: custom enum
{
	IGNOREMSG_NONE,  // Nothing to do
	IGNOREMSG_ENEMY, // To ignore any chat messages from the enemy
	IGNOREMSG_TEAM   // Same as IGNOREMSG_ENEMY but ignore teammates
} IgnoreChatMsg;

typedef enum
{
	UNASSIGNED,
	TERRORIST,
	CT,
	SPECTATOR,
} TeamName;

typedef enum
{
	MODEL_UNASSIGNED,
	MODEL_URBAN,
	MODEL_TERROR,
	MODEL_LEET,
	MODEL_ARCTIC,
	MODEL_GSG9,
	MODEL_GIGN,
	MODEL_SAS,
	MODEL_GUERILLA,
	MODEL_VIP,
	MODEL_MILITIA,
	MODEL_SPETSNAZ
} ModelName;

typedef enum
{
	JOINED,
	SHOWLTEXT,
	READINGLTEXT,
	SHOWTEAMSELECT,
	PICKINGTEAM,
	GETINTOGAME
} JoinState;

typedef enum
{
	CMD_SAY,
	CMD_SAYTEAM,
	CMD_FULLUPDATE,
	CMD_VOTE,
	CMD_VOTEMAP,
	CMD_LISTMAPS,
	CMD_LISTPLAYERS,
	CMD_NIGHTVISION,
	COMMANDS_TO_TRACK,
} TrackCommands;

typedef enum
{
	SILENT,
	CALM,
	INTENSE
} MusicState;

enum
{
	CS_CLASS_NONE = 0,

	// Terrorist classes (keep in sync with FIRST_T_CLASS/LAST_T_CLASS).
	CS_CLASS_PHOENIX_CONNNECTION,
	CS_CLASS_L337_KREW,
	CS_CLASS_ARCTIC_AVENGERS,
	CS_CLASS_GUERILLA_WARFARE,

	// CT classes (keep in sync with FIRST_CT_CLASS/LAST_CT_CLASS).
	CS_CLASS_SEAL_TEAM_6,
	CS_CLASS_GSG_9,
	CS_CLASS_SAS,
	CS_CLASS_GIGN,

	CS_NUM_CLASSES
};

// Keep these in sync with CSClasses.
#define FIRST_T_CLASS	CS_CLASS_PHOENIX_CONNNECTION
#define LAST_T_CLASS	CS_CLASS_GUERILLA_WARFARE

#define FIRST_CT_CLASS	CS_CLASS_SEAL_TEAM_6
#define LAST_CT_CLASS	CS_CLASS_GIGN

// CS Model indexes
#define PLAYERMODELIDX_DEFAULT		0
#define PLAYERMODELIDX_LEET			1	// t
#define PLAYERMODELIDX_GIGN			2	// ct
#define PLAYERMODELIDX_VIP			3	// ct
#define PLAYERMODELIDX_GSG9			4	// ct
#define PLAYERMODELIDX_GUERILLA		5	// t
#define PLAYERMODELIDX_ARCTIC		6	// t
#define PLAYERMODELIDX_SAS			7	// ct
#define PLAYERMODELIDX_TERROR		8	// t
#define PLAYERMODELIDX_URBAN		9	// ct
#define PLAYERMODELIDX_SPETSNAZ		10	// ct
#define PLAYERMODELIDX_MILITIA		11	// t

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT			Vector( 0, 0, 17)

// Height above entity position where the viewer's eye is.
#define VEC_HULL_MIN				Vector(-16, -16, -36)
#define VEC_HULL_MAX				Vector( 16,  16,  36)
#define VEC_HUMAN_HULL_MIN			Vector( -16, -16, 0 )
#define VEC_HUMAN_HULL_MAX			Vector( 16, 16, 72 )
#define VEC_HUMAN_HULL_DUCK			Vector( 16, 16, 36 )

#define VEC_DUCK_HULL_MIN			Vector(-16, -16, -18 )
#define VEC_DUCK_HULL_MAX			Vector( 16,  16,  18)
#define VEC_DUCK_VIEW				Vector( 0, 0, 12 )

#define VECTOR_CONE_1DEGREES		Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES		Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES		Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES		Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES		Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES		Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES		Vector( 0.06105, 0.06105, 0.06105 )	
#define VECTOR_CONE_8DEGREES		Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES		Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES		Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES		Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES		Vector( 0.17365, 0.17365, 0.17365 )

#define	ROUTE_SIZE					8 // how many waypoints a monster can store at one time
#define MAX_OLD_ENEMIES				4 // how many old enemies to remember

#define	itbd_Paralyze				0		
#define	itbd_NerveGas				1
#define	itbd_Poison					2
#define	itbd_Radiation				3
#define	itbd_DrownRecover			4
#define	itbd_Acid					5
#define	itbd_SlowBurn				6
#define	itbd_SlowFreeze				7
#define CDMG_TIMEBASED				8

#define DMG_IMAGE_LIFE				2	// seconds that image is up

#define DMG_IMAGE_POISON			0
#define DMG_IMAGE_ACID				1
#define DMG_IMAGE_COLD				2
#define DMG_IMAGE_DROWN				3
#define DMG_IMAGE_BURN				4
#define DMG_IMAGE_NERVE				5
#define DMG_IMAGE_RAD				6
#define DMG_IMAGE_SHOCK				7
//tf defines
#define DMG_IMAGE_CALTROP			8
#define DMG_IMAGE_TRANQ				9
#define DMG_IMAGE_CONCUSS			10
#define DMG_IMAGE_HALLUC			11
#define NUM_DMG_TYPES				12
// instant damage

#define DMG_GENERIC					0			// generic damage was done
#define DMG_CRUSH					(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET					(1 << 1)	// shot
#define DMG_SLASH					(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN					(1 << 3)	// heat burned
#define DMG_FREEZE					(1 << 4)	// frozen
#define DMG_FALL					(1 << 5)	// fell too far
#define DMG_BLAST					(1 << 6)	// explosive blast damage
#define DMG_CLUB					(1 << 7)	// crowbar, punch, headbutt
#define DMG_SHOCK					(1 << 8)	// electric shock
#define DMG_SONIC					(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM				(1 << 10)	// laser or other high energy beam 
#define DMG_NEVERGIB				(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB				(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.

// time-based damage
//mask off TF-specific stuff too
#define DMG_TIMEBASED				(~(0xff003fff))	// mask for time-based damage


#define DMG_DROWN					(1 << 14)	// Drowning
#define DMG_FIRSTTIMEBASED			DMG_DROWN

#define DMG_PARALYZE				(1 << 15)	// slows affected creature down
#define DMG_NERVEGAS				(1 << 16)	// nerve toxins, very bad
#define DMG_POISON					(1 << 17)	// blood poisioning
#define DMG_RADIATION				(1 << 18)	// radiation exposure
#define DMG_DROWNRECOVER			(1 << 19)	// drowning recovery
#define DMG_ACID					(1 << 20)	// toxic chemicals or acid burns
#define DMG_SLOWBURN				(1 << 21)	// in an oven
#define DMG_SLOWFREEZE				(1 << 22)	// in a subzero freezer
#define DMG_MORTAR					(1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)

//TF ADDITIONS
#define DMG_IGNITE					(1 << 24)	// Players hit by this begin to burn
#define DMG_RADIUS_MAX				(1 << 25)	// Radius damage with this flag doesn't decrease over distance
#define DMG_RADIUS_QUAKE			(1 << 26)	// Radius damage is done like Quake. 1/2 damage at 1/2 radius.
#define DMG_IGNOREARMOR				(1 << 27)	// Damage ignores target's armor
#define DMG_AIMED					(1 << 28)   // Does Hit location damage
#define DMG_WALLPIERCING			(1 << 29)	// Blast Damages ents through walls

#define DMG_CALTROP					(1<<30)
#define DMG_HALLUC					(1<<31)

// TF Healing Additions for TakeHealth
#define DMG_IGNORE_MAXHEALTH		DMG_IGNITE
// TF Redefines since we never use the originals
#define DMG_NAIL					DMG_SLASH
#define DMG_NOT_SELF				DMG_FREEZE

#define DMG_TRANQ					DMG_MORTAR
#define DMG_CONCUSS					DMG_SONIC

#define	MAX_AMMO_TYPES				32		// ???
#define MAX_AMMO_SLOTS				32		// not really slots

#define HUD_PRINTNOTIFY				1
#define HUD_PRINTCONSOLE			2
#define HUD_PRINTTALK				3
#define HUD_PRINTCENTER				4
#define HUD_PRINTRADIO				5

//===================================================================================================================
// Hud Element hiding flags
#define	HIDEHUD_WEAPONS				( 1<<0 )	// Hide viewmodel, ammo count & weapon selection
#define	HIDEHUD_FLASHLIGHT			( 1<<1 )
#define	HIDEHUD_ALL					( 1<<2 )
#define HIDEHUD_HEALTH				( 1<<3 )	// Hide health & armor / suit battery
#define HIDEHUD_PLAYERDEAD			( 1<<4 )	// Hide when local player's dead
#define HIDEHUD_NEEDSUIT			( 1<<5 )	// Hide when the local player doesn't have the HEV suit
#define HIDEHUD_MISCSTATUS			( 1<<6 )	// Hide miscellaneous status elements (trains, pickup history, death notices, etc)
#define HIDEHUD_CHAT				( 1<<7 )	// Hide all communication elements (saytext, voice icon, etc)

#define AUTOAIM_2DEGREES			0.0348994967025		// sin(2)
#define AUTOAIM_5DEGREES			0.08715574274766	// sin(5)
#define AUTOAIM_8DEGREES			0.1391731009601		// sin(8)
#define AUTOAIM_10DEGREES			0.1736481776669		// sin(10)

//------------------------------------------------------------------------------------
// Playermove
//

// texture types
#define CHAR_TEX_SNOW				'N'
#define CHAR_TEX_GRASS				'X'

// texture limits
#define CTEXTURESMAX				1024		// max number of textures loaded
#define CBTEXTURENAMEMAX			17			// only load first n chars of name

//
// Playermove constants
//
#define TIME_TO_DUCK				0.4

#define PM_DEAD_VIEWHEIGHT			-8

#define MAX_CLIMB_SPEED				200

#define STUCK_MOVEUP				1
#define STUCK_MOVEDOWN				-1

// vectors
#define DUCK_HULL_MIN				-18
#define DUCK_HULL_MAX				18
#define DUCK_VIEW					12
#define STANDING_HULL_MIN			-36
#define STANDING_HULL_MAX			36
#define VIEWHEIGHT					28

// epsilon for pm code
#define	STOP_EPSILON				0.1

// stepsounds added in cstrike
#define STEP_SNOW					9  // walking on snowy surface

#define PLAYER_FATAL_FALL_SPEED		1024// approx 60 feet
#define PLAYER_MAX_SAFE_FALL_SPEED	580// approx 20 feet
#define DAMAGE_FOR_FALL_SPEED		(float) 100 / ( PLAYER_FATAL_FALL_SPEED - PLAYER_MAX_SAFE_FALL_SPEED )// damage per unit per second.
#define PLAYER_MIN_BOUNCE_SPEED		200
#define PLAYER_FALL_PUNCH_THRESHHOLD (float)350 // won't punch player's screen/make scrape noise unless player falling at least this fast.

#define PLAYER_LONGJUMP_SPEED		350 // how fast we longjump

#define PLAYER_DUCKING_MULTIPLIER	0.333

// Only allow bunny jumping up to 1.2x server / player maxspeed setting
#define BUNNYJUMP_MAX_SPEED_FACTOR	1.2f

enum HitBoxGroup
{
	HITGROUP_GENERIC = 0,
	HITGROUP_HEAD,
	HITGROUP_CHEST,
	HITGROUP_STOMACH,
	HITGROUP_LEFTARM,
	HITGROUP_RIGHTARM,
	HITGROUP_LEFTLEG,
	HITGROUP_RIGHTLEG,
	HITGROUP_SHIELD,

	NUM_HITGROUPS,
};

#endif // CS_SHAREDDEFS_H
