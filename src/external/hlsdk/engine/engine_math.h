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

#if !defined( ENGINEMATH_H )
#define ENGINEMATH_H
#pragma once

typedef float vec_t;
#if 0
typedef float vec3_t[3];
#else
typedef Vector vec3_t;
#endif
typedef vec_t vec4_t[4];	// x,y,z,w
typedef vec_t vec5_t[5];

#endif // ENGINEMATH_H