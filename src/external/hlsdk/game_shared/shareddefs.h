//========= Copyright (c) 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SHAREDDEFS_H
#define SHAREDDEFS_H
#ifdef _WIN32
#pragma once
#endif

//------------------------------------------------------------------------------------
// Constants
//

#define	ROUTE_SIZE						8 // how many waypoints a monster can store at one time

#define MAX_OLD_ENEMIES					4 // how many old enemies to remember

//------------------------------------------------------------------------------------

#define	bits_CAP_DUCK					( 1 << 0 )// crouch
#define	bits_CAP_JUMP					( 1 << 1 )// jump/leap
#define bits_CAP_STRAFE					( 1 << 2 )// strafe ( walk/run sideways)
#define bits_CAP_SQUAD					( 1 << 3 )// can form squads
#define	bits_CAP_SWIM					( 1 << 4 )// proficiently navigate in water
#define bits_CAP_CLIMB					( 1 << 5 )// climb ladders/ropes
#define bits_CAP_USE					( 1 << 6 )// open doors/push buttons/pull levers
#define bits_CAP_HEAR					( 1 << 7 )// can hear forced sounds
#define bits_CAP_AUTO_DOORS				( 1 << 8 )// can trigger auto doors
#define bits_CAP_OPEN_DOORS				( 1 << 9 )// can open manual doors
#define bits_CAP_TURN_HEAD				( 1 << 10)// can turn head, always bone controller 0

#define bits_CAP_RANGE_ATTACK1			( 1 << 11)// can do a range attack 1
#define bits_CAP_RANGE_ATTACK2			( 1 << 12)// can do a range attack 2
#define bits_CAP_MELEE_ATTACK1			( 1 << 13)// can do a melee attack 1
#define bits_CAP_MELEE_ATTACK2			( 1 << 14)// can do a melee attack 2

#define bits_CAP_FLY					( 1 << 15)// can fly, move all around

#define bits_CAP_DOORS_GROUP			(bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)

//------------------------------------------------------------------------------------

#define	itbd_Paralyze					0		
#define	itbd_NerveGas					1
#define	itbd_Poison						2
#define	itbd_Radiation					3
#define	itbd_DrownRecover				4
#define	itbd_Acid						5
#define	itbd_SlowBurn					6
#define	itbd_SlowFreeze					7
#define CDMG_TIMEBASED					8

//------------------------------------------------------------------------------------
// Menus
// 
//	Note that each mod may have its own set of menus.
//

#define MENU_DEFAULT					1 // unused
#define MENU_TEAM 						2 // team selection
#define MENU_CLASS 						3 // class selection
#define MENU_MAPBRIEFING				4 // level description (maps/<mapname>.txt)
#define MENU_INTRO 						5 // MOTD, map info, ...
#define MENU_CLASSHELP					6
#define MENU_CLASSHELP2 				7
#define MENU_REPEATHELP 				8
#define MENU_SPECHELP					9 // spectator help
// theoretical limit
#define MAX_MENUS						80

//------------------------------------------------------------------------------------
// Playermove materials
//

#define CHAR_TEX_CONCRETE				'C'			// texture types
#define CHAR_TEX_METAL					'M'
#define CHAR_TEX_DIRT					'D'
#define CHAR_TEX_VENT					'V'
#define CHAR_TEX_GRATE					'G'
#define CHAR_TEX_TILE					'T'
#define CHAR_TEX_SLOSH					'S'
#define CHAR_TEX_WOOD					'W'
#define CHAR_TEX_COMPUTER				'P'
#define CHAR_TEX_GLASS					'Y'
#define CHAR_TEX_FLESH					'F'
#define CHAR_TEX_SNOW					'N'

#endif // SHAREDDEFS_H
