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

#ifndef PMAN_FRUSTUM_H
#define PMAN_FRUSTUM_H

#pragma once

class CFrustum
{
public:
	void NormalizeFrustumPlane( float( *frustum )[4], int side );
	void CalculateFrustum();
	bool PointInsideFrustum( float x, float y, float z );
	bool SphereInsideFrustum( float x, float y, float z, float radius );
	bool PlaneInsideFrustum( float x, float y, float z, float size );

	float g_flFrustum[6][4];
};

extern CFrustum g_cFrustum;

#endif