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

#ifndef PMAN_MAIN_H
#define PMAN_MAIN_H

#pragma once

#ifdef _WIN32
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.dll"
#elif defined(OSX)
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.dylib"
#elif defined(LINUX)
#define PARTICLEMAN_DLLNAME "cl_dlls/particleman.so"
#else
#error
#endif

struct ForceMember
{
	int m_iIndex;
	ForceMember* m_pNext, * m_pPrevious;
	Vector m_vOrigin;
	Vector m_vDirection;
	float m_flRadius;
	float m_flStrength;
	float m_flDieTime;
};

struct ForceList
{
	int m_iElements;
	ForceMember* pFirst, * pCurrent, * pLast;
};

class CBaseParticle : public CCoreTriangleEffect
{
public:
	virtual void Think( float time ) { g_pParticleMan->CoreThink( this, time ); }
	virtual void Draw( void ) { g_pParticleMan->CoreDraw( this ); }
	virtual void Animate( float time ) { g_pParticleMan->CoreAnimate( this, time ); }
	virtual void AnimateAndDie( float time ) { g_pParticleMan->CoreAnimateAndDie( this, time ); }
	virtual void Expand( float time ) { g_pParticleMan->CoreExpand( this, time ); }
	virtual void Contract( float time ) { g_pParticleMan->CoreContract( this, time ); }
	virtual void Fade( float time ) { g_pParticleMan->CoreFade( this, time ); }
	virtual void Spin( float time ) { g_pParticleMan->CoreSpin( this, time ); }
	virtual void CalculateVelocity( float time ) { g_pParticleMan->CoreCalculateVelocity( this, time ); }
	virtual void CheckCollision( float time ) { g_pParticleMan->CoreCheckCollision( this, time ); }
	virtual void Touch( Vector pos, Vector normal, int index ) { g_pParticleMan->CoreTouch( this, pos, normal, index ); }
	virtual void Die( void ) { g_pParticleMan->CoreDie( this ); }
	virtual void Force( void ) { g_pParticleMan->CoreForce( this ); }
	virtual bool CheckVisibility( void ) { return g_pParticleMan->CoreCheckVisibility( this ); }

	virtual void InitializeSprite( Vector org, Vector normal, model_t* sprite, float size, float brightness )
	{
		g_pParticleMan->CoreInitializeSprite( this, org, normal, sprite, size, brightness );
	}

	void* operator new(size_t size) //this asks for a new block of memory from the MiniMem class
	{
		return(g_pParticleMan->RequestNewMemBlock( size ));
	}
#ifdef POSIX
	void* operator new(size_t size, const std::nothrow_t&) throw() //this asks for a new block of memory from the MiniMem class
	{
		return(g_pParticleMan->RequestNewMemBlock( size ));
	}
#endif
};

class IParticleMan_Active : public IParticleMan
{
public:
	IParticleMan_Active();
	~IParticleMan_Active();

	void SetUp( cl_enginefunc_t* pEnginefuncs ) override;

	void Update() override;

	void SetVariables( float flGravity, Vector vViewAngles ) override;
	void ResetParticles() override;

	void ApplyForce( Vector vOrigin, Vector vDirection, float flRadius, float flStrength, float flDuration ) override;

	void AddCustomParticleClassSize( unsigned long lSize ) override;
	CBaseParticle* CreateParticle( Vector org, Vector normal, model_t* sprite, float size, float brightness, const char* classname ) override;

	char* RequestNewMemBlock( int iSize ) override;

	void CoreInitializeSprite( CCoreTriangleEffect* pParticle, Vector org, Vector normal, model_t* sprite, float size, float brightness ) override;
	void CoreThink( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreDraw( CCoreTriangleEffect* pParticle ) override;
	void CoreAnimate( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreAnimateAndDie( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreExpand( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreContract( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreFade( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreSpin( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreCalculateVelocity( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreCheckCollision( CCoreTriangleEffect* pParticle, float time ) override;
	void CoreTouch( CCoreTriangleEffect* pParticle, Vector pos, Vector normal, int index ) override;
	void CoreDie( CCoreTriangleEffect* pParticle ) override;
	void CoreForce( CCoreTriangleEffect* pParticle ) override;
	bool CoreCheckVisibility( CCoreTriangleEffect* pParticle ) override;

	void SetRender( int iRender ) override;
};

#endif