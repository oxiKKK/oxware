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
// dlight.h -- Dynamic light entity.
//

#ifndef DLIGHT_H
#define DLIGHT_H
#pragma once

// Dynamic light
typedef struct
{
	vec3_t		origin;			// light center
	float		radius;			// circle radius
	color24		color;
	float		die;			// stop lighting after this time
	float		decay;			// drop this each second
	float		minlight;		// don't add when contributing less
	int			key;
	qboolean	dark;			// subtracts light instead of adding
} dlight_t;

#define MAX_AMBIENTLIGHT	128
#define MAX_SHADELIGHT		192

// Alias model light
typedef struct
{
	int			ambientlight;	// clip at 128
	int			shadelight;		// clip at 192 - ambientlight
	vec3_t		color;			// R, G, B
	float*		plightvec;
} alight_t;

#endif // DLIGHT_H
