/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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
#ifndef USERCMD_H
#define USERCMD_H
#pragma once

// used by usercmd_t
#define IN_ATTACK		(1 << 0)	// +attack
#define IN_JUMP			(1 << 1)	// +jump
#define IN_DUCK			(1 << 2)	// +duck
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)	// +use
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)	// +attack2
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_SCORE		(1 << 15)	// Used by client.dll for when scoreboard is held down

//-----------------------------------------------------------------------------
// Purpose: A user command information being transfered between client&server.
//-----------------------------------------------------------------------------
struct usercmd_t
{
	short	lerp_msec;			// Interpolation time on client. Clamped to 255.
	byte	msec;				// Duration in ms of command
								// This is also frametime is milliseconds.

	vec3_t	viewangles;			// Command view angles.

	//
	// intended velocities
	//

	float	forwardmove;		// Forward velocity.
	float	sidemove;			// Sideways velocity.
	float	upmove;				// Upward velocity.
	byte	lightlevel;			// Light level at spot where we are standing.
	unsigned short buttons;		// Attack buttons
	byte    impulse;			// Impulse command issued.
	byte	weaponselect;		// Current weapon id

	//
	// Experimental player impact stuff.
	//

	int		impact_index;

	vec3_t	impact_position;
};

#endif // USERCMD_H
