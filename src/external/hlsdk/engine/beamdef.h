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
#ifndef BEAMDEF_H
#define BEAMDEF_H
#pragma once

// Start/End Entity is encoded as 12 bits of entity index, and 4 bits 
// of attachment (4:12)
#define BEAMENT_ENTITY(x)		((x)&0xFFF)
#define BEAMENT_ATTACHMENT(x)	(((x)>>12)&0xF)

// Beam types, encoded as a byte
enum
{
	BEAM_POINTS = 0,
	BEAM_ENTPOINT,
	BEAM_ENTS,
	BEAM_HOSE,
};

// curstate.rendermode for the beam entity
#define BEAM_FSINE		0x10
#define BEAM_FSOLID		0x20
#define BEAM_FSHADEIN	0x40
#define BEAM_FSHADEOUT	0x80

#define BEAM_ENTITIES	64

// Flags for beam_t
#define FBEAM_NONE				0
#define FBEAM_STARTENTITY		0x00000001
#define FBEAM_ENDENTITY			0x00000002
#define FBEAM_FADEIN			0x00000004
#define FBEAM_FADEOUT			0x00000008
#define FBEAM_SINENOISE			0x00000010
#define FBEAM_SOLID				0x00000020
#define FBEAM_SHADEIN			0x00000040
#define FBEAM_SHADEOUT			0x00000080
#define FBEAM_STARTVISIBLE		0x10000000 // Has this client actually seen this beam's start entity yet?
#define FBEAM_ENDVISIBLE		0x20000000 // Has this client actually seen this beam's end entity yet?
#define FBEAM_ISACTIVE			0x40000000
#define FBEAM_FOREVER			0x80000000

// Beam structure used by the renderer. Contains some information
// about how the beam behaves and how it should look like.
struct beam_t
{
	beam_t		*next;			// Next entry inside the linked list
	int			type;			// TE_* macros
	int			flags;			// FBEAM_* macros
	vec3_t		source;			// From where the beam starts
	vec3_t		target;			// Where the beam ends
	vec3_t		delta;			// Delta between end and start
	float		t;				// 0 .. 1 over lifetime of beam
	float		freq;			// Frequency
	float		die;			// Die time in seconds
	float		width;			// Width of the beam
	float		amplitude;		// Amplitude of the beam
	float		r, g, b;		// Color of the beam
	float		brightness;		// Brightness of the beam
	float		speed;			// Speed of the beam
	float		frameRate;		// Update frequency
	float		frame;			//
	int			segments;		// How many segments composes the beam
	int			startEntity;	// Index for the starting entity
	int			endEntity;		// Index for the entity that we've hit
	int			modelIndex;		// Model index for precached beam
	int			frameCount;		// Frame count for model
	model_t*	pFollowModel;	//
	particle_t*	particles;		// What particles does this beam creates
};

typedef beam_t BEAM;

#endif // BEAMDEF_H
