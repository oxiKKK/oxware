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

#ifndef CL_ENTITY_H
#define CL_ENTITY_H
#pragma once

// Forward declaration for linked lists
struct cl_entity_t;
struct model_t;
struct mnode_t;

// Entity fragment information
struct efrag_t
{
	mleaf_t*		leaf;
	efrag_t*		leafnext;
	cl_entity_t*	entity;
	efrag_t*		entnext;
};

// Mouth controller used by entities when they're emiting voice sounds.
struct mouth_t
{
	byte	mouthopen;		// 0 = mouth closed, 255 = mouth agape
	byte	sndcount;		// counter for running average
	int		sndavg;			// running average
};

// Variables used by studio model rendering routines
struct latchedvars_t
{
	float	prevanimtime;
	float	sequencetime;
	byte	prevseqblending[2];
	vec3_t	prevorigin;
	vec3_t	prevangles;

	int		prevsequence;
	float	prevframe;

	byte	prevcontroller[4];
	byte	prevblending[2];
};

// Holds the movement time, origin and angles. This is used for entity
// interpolation.
struct position_history_t
{
	float	animtime;	// Timestamp for this movement

	vec3_t	origin;		// Origin and angles saved for this record
	vec3_t	angles;
};

#define HISTORY_MAX		64  // Must be power of 2
#define HISTORY_MASK	( HISTORY_MAX - 1 )

#include "entity_state.h"

// An entity is an active object that is composed out of several
// components described inside this structure. An active list of
// entities is kept inside cl_entities[] array.
struct cl_entity_t
{
	int						index;		// Index into cl_entities ( should match actual slot, but not necessarily )

	qboolean				player;		// True if this entity is a "player" (index > 0 && index < cl.maxclients, see CL_IsPlayerIndex())

	entity_state_t			baseline;	// The original state from which to delta during an uncompressed message
	entity_state_t			prevstate;	// The state information from the penultimate message received from the server
	entity_state_t			curstate;	// The state information from the last message received from server

	int						current_position;		// Last received history update index, starting from one
	position_history_t		ph[HISTORY_MAX];	// History of position and angle updates for this player

	mouth_t					mouth;		// For synchronizing mouth movements.

	latchedvars_t			latched;	// Variables used by studio model rendering routines

	// Information based on interplocation, extrapolation, prediction, or just copied from last msg received.
	float					lastmove;

	// Actual render position and angles
	vec3_t					origin;
	vec3_t					angles;

	// Attachment points
	vec3_t					attachment[4];

	// Other entity local information
	int						trivial_accept;

	model_t*				model;			// cl.model_precache[ curstate.modelindex ];  all visible entities have a model
	efrag_t*				efrag;			// linked list of efrags
	mnode_t*				topnode;		// for bmodels, first world node that splits bmodel, or NULL if not split

	float					syncbase;		// for client-side animations -- used by obsolete alias animation system, remove?
	int						visframe;		// last frame this entity was found in an active leaf
	colorVec				cvFloorColor;
};

// Preserved index for world entity. This is always the first entity 
// in the list -- loaded first.
#define WORLD_ENTITY 0

// created entity type
#define ET_NORMAL		0
#define ET_PLAYER		1
#define ET_TEMPENTITY	2
#define ET_BEAM			3
// BMODEL or SPRITE that was split across BSP nodes
#define ET_FRAGMENTED	4

#endif // CL_ENTITY_H
