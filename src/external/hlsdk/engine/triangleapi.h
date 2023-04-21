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
// triangleapi.h -- TriApi declarations, a wrapper over opengl to be used outside of the engine.
//

#ifndef TRIANGLEAPI_H
#define TRIANGLEAPI_H
#pragma once

enum TRICULLSTYLE
{
	TRI_FRONT = 0,
	TRI_NONE = 1,
};

#define TRI_API_VERSION		1

//-----------------------------------------------------------------------------
// Purpose: Translation modes between opengl and triapi code. See g_GL_Modes[] 
//			for more information.
//-----------------------------------------------------------------------------
#define TRI_TRIANGLES		0 // GL_TRIANGLES
#define TRI_TRIANGLE_FAN	1 // GL_TRIANGLE_FAN
#define TRI_QUADS			2 // GL_QUADS
#define TRI_POLYGON			3 // GL_POLYGON
#define TRI_LINES			4 // GL_LINES
#define TRI_TRIANGLE_STRIP	5 // GL_TRIANGLE_STRIP
#define TRI_QUAD_STRIP		6 // GL_QUAD_STRIP
#define TRI_COUNT			7

struct triangleapi_t
{
	int			version;

	void		(*RenderMode)		(int mode);
	void		(*Begin)			(int primitiveCode);
	void		(*End)				();

	void		(*Color4f)			(float r, float g, float b, float a);
	void		(*Color4ub)			(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void		(*TexCoord2f)		(float u, float v);
	void		(*Vertex3fv)		(float *worldPnt);
	void		(*Vertex3f)			(float x, float y, float z);
	void		(*Brightness)		(float brightness);
	void		(*CullFace)			(TRICULLSTYLE style);
	int			(*SpriteTexture)	(model_t *pSpriteModel, int frame);
	int			(*WorldToScreen)	(float *world, float *screen);  // Returns 1 if it's z clipped
	void		(*Fog)				(float flFogColor[3], float flStart, float flEnd, int bOn); // Works just like GL_FOG, flFogColor is r/g/b.
	void		(*ScreenToWorld)	(float *screen, float *world);
	void		(*GetMatrix)		(const int pname, float *matrix);
	int			(*BoxInPVS)			(float *mins, float *maxs);
	void		(*LightAtPoint)		(float *pos, float *value);
	void		(*Color4fRendermode)(float r, float g, float b, float a, int rendermode);
	void		(*FogParams)		(float flDensity, int iFogSkybox); // Used with Fog()...sets fog density and whether the fog should be applied to the skybox
};

#endif // TRIANGLEAPI_H
