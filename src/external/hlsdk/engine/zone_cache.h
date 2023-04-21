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
#ifndef ZONE_CACHE_H
#define ZONE_CACHE_H
#pragma once

#define CACHE_MAX_NAME 64

struct cache_user_t
{
	void* data;
};

struct cache_system_t
{
	int					size;					// including this header
	cache_user_t*		user;
	char				name[CACHE_MAX_NAME];
	cache_system_t*		prev, * next;
	cache_system_t*		lru_prev, * lru_next;	// for LRU flushing	
};

#endif // ZONE_CACHE_H