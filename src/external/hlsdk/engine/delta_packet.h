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

#ifndef DELTA_PACKET_H
#define DELTA_PACKET_H
#pragma once

#define	MAX_PACKET_ENTITIES			256		// 256 visible entities per frame

#define ENTITY_SENTINEL				9999	// for server and client parsing

typedef struct
{
	int				num_entities;

	// For each entity handled in the to packet, mark that's it has already been deleted if that's the case
	unsigned char	flags[MAX_CLIENTS];
	entity_state_t* entities; // heap allocated entities
} packet_entities_t;

#endif // DELTA_PACKET_H