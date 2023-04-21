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
#ifndef DELTADEF_H
#define DELTADEF_H
#pragma once

// Flags for DELTA Description
#define	FDT_MARK (1 << 0) // Delta mark for sending

// Conditional encoder routine
typedef void (*pfnencoder_t)(struct delta_t*, const byte* from, const byte* to);

// Holds information about structure fields.
struct delta_definition_t
{
	char*	fieldName;		// Name of the struct member
	size_t	fieldOffset;	// offset from base of the struct to the member
							// (Calculated with macro offsetof())
};

//This serves sort of like a dictionary for delta definitions
struct delta_definition_list_t
{
	delta_definition_list_t* next;
	char*					 ptypename;		// Name of the definition list
	int						 numelements;	// Number of elements inside this list

	delta_definition_t*		 pdefinition;	// Pointer to heap-allocated definition
};

struct delta_registry_t
{
	delta_registry_t*	next;

	char*				name;	// heap-allocated name for this registry
	delta_t*			pdesc;
};

struct delta_encoder_t
{
	delta_encoder_t*	next;
	char*				name;

	pfnencoder_t		conditionalencode;
};

// Used to keep track of how many data was sent and received.
struct delta_stats_t
{
	int sendcount;		// Sent fields that are marked with FDT_MARK
	int receivedcount;
};

#define DELTA_DESCRIPTOR_NAME_MAX 32

struct delta_description_t
{
	int				fieldType;

	char			fieldName[DELTA_DESCRIPTOR_NAME_MAX];
	int				fieldOffset;

	short			fieldSize;
	int				significant_bits;

	// Some fields are being multiplied with this
	float			premultiply, postmultiply;

	short			flags;
	delta_stats_t	stats;
};

struct delta_link_t
{
	delta_link_t*		 next;
	delta_description_t* delta;
};

#define DELTA_CONROUTINE_NAME_MAX 32

struct delta_t
{
	// True if descriptor is heap-allocated
	qboolean				dynamic;

	// Count of individual fields inside this delta
	int						fieldCount;

	// Conditional encoder fields
	char					conditionalencodename[DELTA_CONROUTINE_NAME_MAX];
	pfnencoder_t			conditionalencode;

	// Descriptor for current delta
	delta_description_t*	pdd;
};

struct delta_info_t
{
	delta_info_t*	next;

	char*			name;		// Name of delta
	char*			loadfile;	// Name of a load file (delta.lst)

	delta_t*		delta;
};

#endif // DELTADEF_H
