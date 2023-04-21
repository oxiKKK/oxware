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
