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

//
// save.h -- everything related to server data saving/restoring
//

#ifndef SAVE_H
#define SAVE_H
#pragma once

#define SAVEFILE_HEADER		MAKEID('V','A','L','V')		// little-endian "VALV"
#define SAVEGAME_HEADER		MAKEID('J','S','A','V')		// little-endian "JSAV"
#define SAVEGAME_VERSION	0x0071						// Version 0.71

typedef void(*SV_SAVEGAMECOMMENT_FUNC)(char*, int);

// Those are just a save data structs, which are responsible for saving/restoring
// game data when the "save" button has been pressed
struct SAVE_HEADER
{
	int			saveId;				// Save ID
	int			version;			// Version
	int			skillLevel;			// Skill Level
	int			entityCount;		// Entity Count
	int			connectionCount;	// Connection Count
	int			lightStyleCount;

	float		time;				// Time

	char		mapName[32];		// Map Name
	char		skyName[32];		// Sky Name

	// Sky lighting colors
	int			skyColor_r;
	int			skyColor_g;
	int			skyColor_b;

	// Sky vectors
	float		skyVec_x;
	float		skyVec_y;
	float		skyVec_z;
};

struct GAME_HEADER
{
	char		mapName[32];
	char		comment[80];

	int			mapCount;
};

struct SAVELIGHTSTYLE
{
	int			index;
	char		style[MAX_LIGHTSTYLES];
};

// Passed to pfnKeyValue
struct KeyValueData
{
	char*		szClassName;	// in: entity classname
	char*		szKeyName;		// in: name of key
	char*		szValue;		// in: value of key
	int32		fHandled;		// out: DLL sets to true if key-value pair was understood
};

struct LEVELLIST
{
	char		mapName[32];
	char		landmarkName[32];
	edict_t*	pentLandmark;
	vec3_t		vecLandmarkOrigin;
};

#define MAX_LEVEL_CONNECTIONS	16		// These are encoded in the lower 16bits of ENTITYTABLE->flags

struct ENTITYTABLE
{
	int			id;				// Ordinal ID of this entity (used for entity <--> pointer conversions)
	edict_t*	pent;			// Pointer to the in-game entity

	int			location;		// Offset from the base data of this entity
	int			size;			// Byte size of this entity's data
	int			flags;			// This could be a short -- bit mask of transitions that this entity is in the PVS of
	string_t	classname;		// entity class name
};

#define FENTTABLE_PLAYER		0x80000000
#define FENTTABLE_REMOVED		0x40000000
#define FENTTABLE_MOVEABLE		0x20000000
#define FENTTABLE_GLOBAL		0x10000000

struct SAVERESTOREDATA
{
	char*		pBaseData;		// Start of all entity save data
	char*		pCurrentData;	// Current buffer pointer for sequential access
	int			size;			// Current data size
	int			bufferSize;		// Total space for data
	int			tokenSize;		// Size of the linear list of tokens
	int			tokenCount;		// Number of elements in the pTokens table
	char**		pTokens;		// Hash table of entity strings (sparse)
	int			currentIndex;	// Holds a global entity table ID
	int			tableCount;		// Number of elements in the entity table
	int			connectionCount;// Number of elements in the levelList[]
	ENTITYTABLE* pTable;		// Array of ENTITYTABLE elements (1 for each entity)
	LEVELLIST	levelList[MAX_LEVEL_CONNECTIONS];		// List of connections from this level

	// smooth transition
	int			fUseLandmark;
	char		szLandmarkName[20];// landmark we'll spawn near in next level
	vec3_t		vecLandmarkOffset;// for landmark transitions
	float		time;
	char		szCurrentMapName[32];	// To check global entities

};

enum EI_FIELDTYPE
{
	EI_FIELD_FLOAT = 0,			// Any floating point value
	EI_FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	EI_FIELD_ENTITY,			// An entity offset (EOFFSET)
	EI_FIELD_CLASSPTR,			// CBaseEntity *
	EI_FIELD_EHANDLE,			// Entity handle
	EI_FIELD_EVARS,				// EVARS *
	EI_FIELD_EDICT,				// edict_t *, or edict_t *  (same thing)
	EI_FIELD_VECTOR,			// Any vector
	EI_FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	EI_FIELD_POINTER,			// Arbitrary data pointer... to be removed, use an array of FIELD_CHARACTER
	EI_FIELD_INTEGER,			// Any integer or enum
	EI_FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)
	EI_FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	EI_FIELD_SHORT,				// 2 byte integer
	EI_FIELD_CHARACTER,			// a byte
	EI_FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	EI_FIELD_MODELNAME,			// Engine string that is a model name (needs precache)
	EI_FIELD_SOUNDNAME,			// Engine string that is a sound name (needs precache)

	EI_FIELD_TYPE_COUNT,		// MUST BE LAST
};

#if !defined(offsetof)  && !defined(GNUC)
#define offsetof(s,m)	(size_t)&(((s *)0)->m)
#endif

#define EI_FIELD(type,name,fieldtype,count,flags)		{ fieldtype, #name, offsetof(type, name), count, flags }
#define EI_DEFINE_FIELD(type,name,fieldtype)			EI_FIELD(type, name, fieldtype, 1, 0)
#define EI_DEFINE_ARRAY(type,name,fieldtype,count)		EI_FIELD(type, name, fieldtype, count, 0)
#define EI_DEFINE_ENTITY_FIELD(name,fieldtype)			EI_FIELD(entvars_t, name, fieldtype, 1, 0 )
#define EI_DEFINE_ENTITY_GLOBAL_FIELD(name,fieldtype)	EI_FIELD(entvars_t, name, fieldtype, 1, EI_FTYPEDESC_GLOBAL )
#define EI_DEFINE_GLOBAL_FIELD(type,name,fieldtype)		EI_FIELD(type, name, fieldtype, 1, EI_FTYPEDESC_GLOBAL )

#define EI_FTYPEDESC_GLOBAL			0x0001		// This field is masked for global entity save/restore

struct TYPEDESCRIPTION
{
	EI_FIELDTYPE	fieldType;
	const char*		fieldName;
	int				fieldOffset;
	short			fieldSize;
	short			flags;
};

#endif