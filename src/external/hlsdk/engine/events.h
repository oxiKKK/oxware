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
#ifndef EVENTS_H
#define EVENTS_H
#pragma once

//
// Args
//

// Event was invoked with stated origin
#define FEVENT_ORIGIN	( 1<<0 )

// Event was invoked with stated angles
#define FEVENT_ANGLES	( 1<<1 )

typedef struct
{
	int		flags;

	// Transmitted
	int		entindex;		// Stores flags when playbacking a clientside event

	vec3_t	origin;
	vec3_t	angles;
	vec3_t	velocity;

	int		ducking;

	float	fparam1;
	float	fparam2;

	int		iparam1;
	int		iparam2;

	int		bparam1;
	int		bparam2;
} event_args_t;

typedef void(*event_dispatchfn_t)(event_args_t*);

//
// Flags
//

#define FEV_NONE				0

// Skip local host for event send.
#define FEV_NOTHOST				(1<<0)    

// Send the event reliably.  You must specify the origin and angles and use
// EngPlaybackEvent for this to work correctly on the server for anything
// that depends on the event origin/angles.  I.e., the origin/angles are not
// taken from the invoking edict for reliable events.
#define FEV_RELIABLE			(1<<1)	 

// Don't restrict to PAS/PVS, send this event to _everybody_ on the server ( useful for stopping CHAN_STATIC
//  sounds started by client event when client is not in PVS anymore ( hwguy in TFC e.g. ).
#define FEV_GLOBAL				(1<<2)

// If this client already has one of these events in its queue, just update the event instead of sending it as a duplicate
//
#define FEV_UPDATE				(1<<3)

// Only send to entity specified as the invoker
#define	FEV_HOSTONLY			(1<<4)

// Only send if the event was created on the server.
#define FEV_SERVER				(1<<5)

// Only issue event client side ( from shared code )
#define FEV_CLIENT				(1<<6)

#define DEFAULT_EVENT_RESENDS	1

typedef struct
{
	unsigned short	index;			// 0 implies not in use

	short			packet_index;	// Use data from state info for entity in delta_packet. -1 implies separate info based on event
									// parameter signature
	short			entity_index;	// The edict this event is associated with

	float			fire_time;		// if non-zero, the time when the event should be fired ( fixed up on the client )

	event_args_t	args;

	// CLIENT ONLY	
	int				flags;			// Reliable or not, etc.
} event_info_t;

typedef struct
{
	event_info_t ei[MAX_EVENT_QUEUE];
} event_state_t;

typedef struct
{
	unsigned short	index;
	const char*		filename;
	int				filesize;
	const char*		pszScript;
} event_t;

//
// API
//

#define EVENT_API_VERSION 1

struct physent_t;

typedef struct
{
	int		version;
	void	(*EV_PlaySound) (int ent, float* origin, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch);
	void	(*EV_StopSound) (int ent, int channel, const char* sample);
	int		(*EV_FindModelIndex)(const char* pmodel);
	int		(*EV_IsLocal) (int playernum);
	int		(*EV_LocalPlayerDucking) (void);
	void	(*EV_LocalPlayerViewheight) (float* viewheight);
	void	(*EV_LocalPlayerBounds) (int hull, float* mins, float* maxs);
	int		(*EV_IndexFromTrace) (struct pmtrace_t* pTrace);
	physent_t* (*EV_GetPhysent) (int idx);
	void	(*EV_SetUpPlayerPrediction) (int dopred, int bIncludeLocalClient);
	void	(*EV_PushPMStates) (void);
	void	(*EV_PopPMStates) (void);
	void	(*EV_SetSolidPlayers) (int playernum);
	void	(*EV_SetTraceHull) (int hull);
	void	(*EV_PlayerTrace) (float* start, float* end, int traceFlags, int ignore_pe, struct pmtrace_t* tr);
	void	(*EV_WeaponAnimation) (int sequence, int body);
	unsigned short (*EV_PrecacheEvent) (int type, const char* psz);
	void	(*EV_PlaybackEvent) (int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	const char* (*EV_TraceTexture) (int ground, float* vstart, float* vend);
	void	(*EV_StopAllSounds) (int entnum, int entchannel);
	void    (*EV_KillEvents) (int entnum, const char* eventname);
} event_api_t;

extern event_api_t eventapi;

#endif // EVENTS_H
