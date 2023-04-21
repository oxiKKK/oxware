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

#ifndef TRI_H
#define TRI_H
#pragma once

//-----------------------------------------------------------------------------
class CPartSmokeGrenade : public CBaseParticle
{
public:
	virtual void Think(float flTime);
	virtual void Force();

	static CPartSmokeGrenade* Create(Vector org, Vector normal, model_t *sprite, float size, float brightness, const char *classname);

public:
	bool	m_bInsideSmoke;
	bool	m_bSpawnInside;
	bool	m_bDampMod;
	Vector	m_vRingOrigin;
	bool	m_bAffectedByForce;
	int		bdsg;
	int		dsgads;
};

//-----------------------------------------------------------------------------
class CPartGrassPiece : public CBaseParticle
{
public:
	virtual void Think(float flTime);
};

//-----------------------------------------------------------------------------
class CPartSnowFlake : public CBaseParticle
{
public:
	virtual void Touch(Vector pos, Vector normal, int index);
	virtual void Think(float time);

public:
	bool	m_bSpiral; 
	bool	m_bTouched; 
	float	m_flSpiralTime; 
};

//-----------------------------------------------------------------------------
class CPartRainDrop : public CBaseParticle
{
public:
	virtual void Think(float time);
	virtual void Touch(Vector pos, Vector normal, int index);

public:
	bool m_bTouched;
};

//-----------------------------------------------------------------------------
class CPartWind : public CBaseParticle
{
public:
	virtual void Think(float time);
};

//-----------------------------------------------------------------------------

void CreateGasSmoke(Vector origin, Vector vVelocity, bool bInsideSmoke, bool bSpawnInside, bool bBlowable);
void CreateGasInside(Vector origin);

#endif //TRI_H