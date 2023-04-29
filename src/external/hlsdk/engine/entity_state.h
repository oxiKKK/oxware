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
#if !defined( ENTITY_STATEH )
#define ENTITY_STATEH
#ifdef _WIN32
#pragma once
#endif

#include "weaponinfo.h"

// For entityType below
#define ENTITY_NORMAL		(1<<0)
#define ENTITY_BEAM			(1<<1)

// Entity state is used for the baseline and for delta compression of a packet of 
//  entities that is sent to a client.

struct entity_state_t
{
// Fields which are filled in by routines outside of delta compression
	int			entityType;
	// Index into cl_entities array for this entity.
	int			number;

	// Timestamp when new entity packet has been received
	float		msg_time;

	// Message number last time the player/entity state was updated.
	// In other words - cl.parsecount
	int			messagenum;

	// Fields which can be transitted and reconstructed over the network stream
	vec3_t		origin;
	vec3_t		angles;

	int			modelindex;
	int			sequence;
	float		frame;
	int			colormap;
	short		skin;
	short		solid;
	int			effects;
	float		scale; // Holds gl_wateramp value

	byte		eflags;

	// Render information
	int			rendermode;
	int			renderamt;
	color24		rendercolor; // For world entity this is gWaterColor
	int			renderfx;

	int			movetype;
	float		animtime;
	float		framerate;
	int			body;			// Object bodygroup
	byte		controller[4];
	byte		blending[4];

	vec3_t		velocity;

	// Send bbox down to client for use during prediction.
	vec3_t		mins;
	vec3_t		maxs;

	int			aiment; // Set by the server dll
	// If owned by a player, the index of that player ( for projectiles ).
	int			owner;

	// Friction, for prediction.
	float		friction;
	// Gravity multiplier
	float		gravity;

// PLAYER SPECIFIC
	int			team;
	int			playerclass;
	int			health;
	qboolean	spectator;
	int         weaponmodel;
	int			gaitsequence;

	// If standing on conveyor, e.g.
	vec3_t		basevelocity;

	// Use the crouched hull, or the regular player hull.
	int			usehull;
	// Latched buttons last time state updated.
	int			oldbuttons;
	// -1 = in air, else pmove entity number
	int			onground;
	int			iStepLeft;
	// How fast we are falling
	float		flFallVelocity;

	float		fov;
	int			weaponanim;

	// Parametric movement overrides
	vec3_t		startpos;
	vec3_t		endpos;

	float		impacttime;
	float		starttime;

	// For mods
	int			iuser1;
	int			iuser2;
	int			iuser3; // cstrike bit flags: PLAYER_* macros (e.g. PLAYER_CAN_SHOOT, PLAYER_FREEZE_TIME_OVER, etc.)
	int			iuser4; // Light multiplier for CS:CZ
	float		fuser1; // smoke size in CS
	float		fuser2;
	float		fuser3;
	float		fuser4;
	vec3_t		vuser1;
	vec3_t		vuser2;
	vec3_t		vuser3;
	vec3_t		vuser4;
};

struct clientdata_t
{
	vec3_t				origin;
	vec3_t				velocity;

	int					viewmodel;

	vec3_t				punchangle;

	int					flags;			// g_iPlayerFlags in cstrike
	int					waterlevel;		// g_iWaterLevel in cstrike
	int					watertype;

	vec3_t				view_ofs;

	float				health;

	int					bInDuck;

	int					weapons; // remove?

	int					flTimeStepSound;
	int					flDuckTime;
	int					flSwimTime;
	int					waterjumptime;

	float				maxspeed;

	float				fov;
	int					weaponanim;

	int					m_iId;
	int					ammo_shells;
	int					ammo_nails;
	int					ammo_cells;
	int					ammo_rockets;
	float				m_flNextAttack;

	int					tfstate;

	int					pushmsec;

	int					deadflag;

	char				physinfo[MAX_PHYSINFO_STRING];

	// For mods
	int					iuser1;
	int					iuser2;
	int					iuser3;		// cstrike bit flags: PLAYER_* macros (e.g. PLAYER_CAN_SHOOT, PLAYER_FREEZE_TIME_OVER, etc.)
	int					iuser4;
	float				fuser1;
	float				fuser2;
	float				fuser3;
	float				fuser4;
	vec3_t				vuser1;
	vec3_t				vuser2;
	vec3_t				vuser3;
	vec3_t				vuser4;
};

typedef struct local_state_s
{
	entity_state_t playerstate;
	clientdata_t   client;
	weapon_data_t  weapondata[MULTIPLAYER_BACKUP];
} local_state_t;

// Default minimum number of clients for multiplayer servers
#define MP_MIN_CLIENTS		6

#define BASELINE_BITS		6
#define NUM_BASELINES		(1<<BASELINE_BITS)

#define BASELINE_BIG_BITS	11
#define NUM_BASELINES_BIG	(1<<BASELINE_BIG_BITS)

struct extra_baselines_t
{
	int number;
	int classname[NUM_BASELINES];

	entity_state_t baseline[NUM_BASELINES];
};

#endif // !ENTITY_STATEH
