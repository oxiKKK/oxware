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
// custom.h -- Client resource customization
//

#ifndef CUSTOMIZATION_H
#define CUSTOMIZATION_H
#pragma once

#define MAX_RESOURCE_NAME 64

// type of the resource
enum resourcetype_t
{
	t_sound = 0,	// Sound type (.wav)
	t_skin,			// 
	t_model,		// Studio model (.mdl)
	t_decal,		// Decal wad file (.wad)
	t_generic,
	t_eventscript,	// Event such as gun shot (.sc)
	t_world,		// Fake type for world, is really t_model

	t_count = 8		// 8
};

struct _resourceinfo_t
{
	int				size;
};

struct resourceinfo_t
{
	_resourceinfo_t info[t_count];
};

// resource flags
#define RES_FATALIFMISSING	(1 << 0) // Disconnect if we can't get this file.
#define RES_PRELOAD			(1 << 1) // Load on client rather than just reserving name
#define RES_CUSTOM			(1 << 2) // Is this resource one that corresponds to another player's customization
									 // or is it a server startup resource.
#define RES_REQUESTED		(1 << 3) // Already requested a download of this one
#define RES_PRECACHED		(1 << 4) // Already precached
#define RES_ALWAYS			(1 << 5) // download always even if available on client	
#define RES_UNKNOWN_6		(1 << 6) // TODO: what's that?
#define RES_CHECKFILE		(1 << 7) // check file on client

// resource_t::rguc_reserved consistency information offsets
#define conoff_type			0
#define conoff_mins			1
#define conoff_maxs			13

// A linked list of resources
struct resource_t
{
	char			szFileName[MAX_RESOURCE_NAME]; // File name to download/precache.
	resourcetype_t	type;				// t_sound, t_skin, t_model, t_decal.
	int				nIndex;				// For t_decals
	int				nDownloadSize;		// Size in Bytes if this must be downloaded.
	unsigned char	ucFlags;

	// For handling client to client resource propagation
	unsigned char	rgucMD5_hash[16];	// To determine if we already have it.
	unsigned char	playernum;			// Which player index this resource is associated with, if it's a custom resource.

	unsigned char	rguc_reserved[32];	// [0]		== consistency_t::check_type
										// [1-12]	== mins get from R_GetStudioBounds
										// [13-24]	== maxs get from R_GetStudioBounds
										// [25-31]	== unused
										// Gets munged by the server after its filled with data.

	resource_t*		pNext;				// Next in chain.
	resource_t*		pPrev;
};

// client customization
struct customization_t
{
	qboolean		bInUse;			// Is this customization in use;
	resource_t		resource;		// The resource_t for this customization
	qboolean		bTranslated;	// Has the raw data been translated into a useable format?  
									//  (e.g., raw decal .wad make into texture_t *)
	int				nUserData1;		// Customization specific data
	int				nUserData2;		// Customization specific data
	void*			pInfo;			// Buffer that holds the data structure that references the data (e.g., the cachewad_t)
	void*			pBuffer;		// Buffer that holds the data for the customization (the raw .wad data)
	customization_t* pNext;			// Next in chain
};

// customization flags
#define FCUST_FROMHPAK		(1 << 0)
#define FCUST_WIPEDATA		(1 << 1)
#define FCUST_IGNOREINIT	(1 << 2)
#define FCUST_VALIDATED		(1 << 3)

// com_custom.c
void		COM_ClearCustomizationList(customization_t *pHead, qboolean bCleanDecals);
qboolean	COM_CreateCustomization(customization_t *pListHead, resource_t *pResource, int playernumber, int flags,
				customization_t **pCustomization, int *nLumps);
int			COM_SizeofResourceList(resource_t *pList, resourceinfo_t *ri);

#endif // CUSTOMIZATION_H
