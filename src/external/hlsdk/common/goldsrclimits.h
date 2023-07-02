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

#ifndef GOLDSRC_LIMITS_H
#define GOLDSRC_LIMITS_H

#if defined(_WIN32)
#pragma once
#endif

// DATA STRUCTURE INFO

#define MAX_NUM_ARGVS				50

#define COM_TOKEN_LEN				2048

// SYSTEM INFO
#define MAX_QPATH					64		// max length of a game pathname
#define MAX_PATH					260

// cbuf constants
#define CMD_MAX_TOKEN_LENGTH		516
#define CMD_MAX_LINE				1024
#define CMD_MAX_LENGTH				256
#define CMD_MAX_BUFFER				0x1000	// 4,096

#define RELEASE_DAY					34995	// Possibly Thursday, October 24, 1996

//
// GoldSrc limits
//

// lightstyles
#define MAX_LIGHTSTYLE_INDEX_BITS	6
#define MAX_LIGHTSTYLES				(1 << MAX_LIGHTSTYLE_INDEX_BITS)

// entity dictionary
#define MAX_EDICT_BITS				11
#define MAX_EDICTS					900

// models
#define MAX_MODEL_INDEX_BITS		9	// sent as a short
#define MAX_MODELS					(1 << MAX_MODEL_INDEX_BITS)

// sound
#define MAX_SOUND_INDEX_BITS		9
#define MAX_SOUNDS					(1 << MAX_SOUND_INDEX_BITS)

// sound hash lookup table
#define SOUNDS_HASHLOOKUP_SIZE		(MAX_SOUNDS * 2 - 1)

// generic
#define MAX_GENERIC_INDEX_BITS		9
#define MAX_GENERIC					(1 << MAX_GENERIC_INDEX_BITS)

// decals
#define MAX_DECAL_INDEX_BITS		9
#define MAX_BASE_DECALS				(1 << MAX_DECAL_INDEX_BITS)

// events
#define MAX_EVENT_INDEX_BITS		8
#define MAX_EVENTS					(1 << MAX_EVENT_INDEX_BITS)

// event queues
#define MAX_EVENT_QUEUE_BITS		6
#define MAX_EVENT_QUEUE				(1 << MAX_EVENT_QUEUE_BITS) // 64 simultaneous events, max

// consistency
#define MAX_CONSISTENCY_BITS		9
#define MAX_CONSISTENCY				(1 << MAX_CONSISTENCY_BITS)

// resources
#define MAX_RESOURCE_INDEX_BITS		12
#define MAX_RESOURCES				(MAX_MODELS + MAX_SOUNDS + MAX_EVENTS)

// client weapons
#define MAX_CLIENT_WEAPON_BITS		6
#define MAX_CLIENT_WEAPONS			(1 << MAX_CLIENT_WEAPON_BITS)

// clients playing on the server
#define MAX_CLIENT_BITS				5
#define MAX_CLIENTS					(1 << MAX_CLIENT_BITS) // Max # of clients allowed in a server.

// entity fragments
#define	MAX_EFRAGS					640

// visible ents
#define MAX_VISEDICT_BITS			9
#define MAX_VISEDICTS				(1 << MAX_VISEDICT_BITS)

// sound effects
#define MAX_SFX_BITS				10
#define	MAX_SFX						(10 << MAX_SFX_BITS)

//
// Texture information
//

#define MAX_LIGHTMAPS				4

#define MAX_HULLS					4

// build info
// day counter from 10/24/96
extern int build_number(void);

#endif // GOLDSRC_LIMITS_H