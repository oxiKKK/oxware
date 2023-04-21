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
#if !defined ( R_EFXH )
#define R_EFXH
#pragma once

#include "particledef.h"
#include "beamdef.h"
#include "light.h"

struct tempent_t
{
	int			flags;
	float		die;
	float		frameMax;
	float		x, y, z;
	float		fadeSpeed;
	float		bounceFactor;
	int			hitSound;
	void		(*hitcallback)	(tempent_t* ent, struct pmtrace_t* ptr);
	void		(*callback)		(tempent_t* ent, float frametime, float currenttime);
	tempent_t*	next;
	int			priority;
	short		clientIndex;	// if attached, this is the index of the client to stick to
								// if COLLIDEALL, this is the index of the client to ignore
								// TENTS with FTENT_PLYRATTACHMENT MUST set the clientindex! 

	vec3_t		tentOffset;		// if attached, client origin + tentOffset = tent origin.
	cl_entity_t	entity;

	// baseline.origin		- velocity
	// baseline.renderamt	- starting fadeout intensity
	// baseline.angles		- angle velocity
};

using TEMPENTITY = tempent_t;

struct efx_api_t
{
	particle_t* (*R_AllocParticle)				(void (*callback) (particle_t* particle, float frametime));
	void		(*R_BlobExplosion)				(vec_t*org);
	void		(*R_Blood)						(vec_t*org, vec_t*dir, int pcolor, int speed);
	void		(*R_BloodSprite)				(vec_t*org, int colorindex, int modelIndex, int modelIndex2, float size);
	void		(*R_BloodStream)				(vec_t*org, vec_t*dir, int pcolor, int speed);
	void		(*R_BreakModel)					(vec_t*pos, vec_t*size, vec_t*dir, float random, float life, int count, int modelIndex, char flags);
	void		(*R_Bubbles)					(vec_t*mins, vec_t*maxs, float height, int modelIndex, int count, float speed);
	void		(*R_BubbleTrail)				(vec_t*start, vec_t*end, float height, int modelIndex, int count, float speed);
	void		(*R_BulletImpactParticles)	(vec_t*pos);
	void		(*R_EntityParticles)			(cl_entity_t* ent);
	void		(*R_Explosion)				(vec_t*pos, int model, float scale, float framerate, int flags);
	void		(*R_FizzEffect)				(cl_entity_t* pent, int modelIndex, int density);
	void		(*R_FireField) 				(vec_t*org, int radius, int modelIndex, int count, int flags, float life);
	void		(*R_FlickerParticles)			(vec_t*org);
	void		(*R_FunnelSprite)				(vec_t*org, int modelIndex, int reverse);
	void		(*R_Implosion)				(vec_t*end, float radius, int count, float life);
	void		(*R_LargeFunnel)				(vec_t*org, int reverse);
	void		(*R_LavaSplash)				(vec_t*org);
	void		(*R_MultiGunshot)				(vec_t*org, vec_t*dir, vec_t*noise, int count, int decalCount, int* decalIndices);
	void		(*R_MuzzleFlash)				(vec_t*pos1, int type);
	void		(*R_ParticleBox)				(vec_t*mins, vec_t*maxs, unsigned char r, unsigned char g, unsigned char b, float life);
	void		(*R_ParticleBurst)			(vec_t*pos, int size, int color, float life);
	void		(*R_ParticleExplosion)		(vec_t*org);
	void		(*R_ParticleExplosion2)		(vec_t*org, int colorStart, int colorLength);
	void		(*R_ParticleLine)				(vec_t*start, vec_t*end, unsigned char r, unsigned char g, unsigned char b, float life);
	void		(*R_PlayerSprites)			(int client, int modelIndex, int count, int size);
	void		(*R_Projectile)				(vec_t*origin, vec_t* velocity, int modelIndex, int life, int owner, void (*hitcallback)(tempent_t* ent, struct pmtrace_t* ptr));
	void		(*R_RicochetSound)			(vec_t*pos);
	void		(*R_RicochetSprite)			(vec_t*pos, model_t* pmodel, float duration, float scale);
	void		(*R_RocketFlare)				(vec_t*pos);
	void		(*R_RocketTrail)				(vec_t* start, vec_t*end, int type);
	void		(*R_RunParticleEffect)		(vec_t*org, vec_t*dir, int color, int count);
	void		(*R_ShowLine)					(vec_t*start, vec_t*end);
	void		(*R_SparkEffect)				(vec_t*pos, int count, int velocityMin, int velocityMax);
	void		(*R_SparkShower)				(vec_t*pos);
	void		(*R_SparkStreaks)				(vec_t*pos, int count, int velocityMin, int velocityMax);
	void		(*R_Spray)					(vec_t*pos, vec_t*dir, int modelIndex, int count, int speed, int spread, int rendermode);
	void		(*R_Sprite_Explode)			(tempent_t* pTemp, float scale, int flags);
	void		(*R_Sprite_Smoke)				(tempent_t* pTemp, float scale);
	void		(*R_Sprite_Spray)				(vec_t*pos, vec_t*dir, int modelIndex, int count, int speed, int iRand);
	void		(*R_Sprite_Trail)				(int type, vec_t*start, vec_t*end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed);
	void		(*R_Sprite_WallPuff)			(tempent_t* pTemp, float scale);
	void		(*R_StreakSplash)				(vec_t*pos, vec_t*dir, int color, int count, float speed, int velocityMin, int velocityMax);
	void		(*R_TracerEffect)				(vec_t* start, vec_t* end);
	void		(*R_UserTracerParticle)		(vec_t*org, vec_t*vel, float life, int colorIndex, float length, unsigned char deathcontext, void (*deathfunc)(particle_t* particle));
	particle_t* (*R_TracerParticles)			(vec_t*org, vec_t*vel, float life);
	void		(*R_TeleportSplash)			(vec_t*org);
	void		(*R_TempSphereModel)			(vec_t*pos, float speed, float life, int count, int modelIndex);
	tempent_t* (*R_TempModel)					(vec_t*pos, vec_t*dir, vec_t*angles, float life, int modelIndex, int soundtype);
	tempent_t* (*R_DefaultSprite)				(vec_t*pos, int spriteIndex, float framerate);
	tempent_t* (*R_TempSprite)					(vec_t*pos, vec_t*dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags);
	int			(*Draw_DecalIndex)			(int id);
	int			(*Draw_DecalIndexFromName)	(char* name);
	void		(*R_DecalShoot)				(int textureIndex, int entity, int modelIndex, vec_t*position, int flags);
	void		(*R_AttachTentToPlayer)		(int client, int modelIndex, float zoffset, float life);
	void		(*R_KillAttachedTents)		(int client);
	beam_t* (*R_BeamCirclePoints)		(int type, vec_t*start, vec_t*end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b);
	beam_t* (*R_BeamEntPoint)			(int startEnt, vec_t*end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b);
	beam_t* (*R_BeamEnts)				(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b);
	beam_t* (*R_BeamFollow)				(int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness);
	void		(*R_BeamKill)					(int deadEntity);
	beam_t* (*R_BeamLightning)			(vec_t*start, vec_t*end, int modelIndex, float life, float width, float amplitude, float brightness, float speed);
	beam_t* (*R_BeamPoints)				(vec_t*start, vec_t*end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b);
	beam_t* (*R_BeamRing)				(int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b);
	dlight_t* (*CL_AllocDlight)			(int key);
	dlight_t* (*CL_AllocElight)			(int key);
	tempent_t* (*CL_TempEntAlloc)			(vec_t*org, model_t* model);
	tempent_t* (*CL_TempEntAllocNoModel)	(vec_t*org);
	tempent_t* (*CL_TempEntAllocHigh)		(vec_t*org, model_t* model);
	tempent_t* (*CL_TentEntAllocCustom)		(vec_t*origin, model_t* model, int high, void (*callback) (tempent_t* ent, float frametime, float currenttime));
	void		(*R_GetPackedColor)			(short* packed, short color);
	short		(*R_LookupColor)				(unsigned char r, unsigned char g, unsigned char b);
	void		(*R_DecalRemoveAll)			(int textureIndex); //textureIndex points to the decal index in the array, not the actual texture index.
	void		(*R_FireCustomDecal)			(int textureIndex, int entity, int modelIndex, vec_t*position, int flags, float scale);
};

#endif
