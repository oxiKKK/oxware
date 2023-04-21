/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

//
// spritegn.h -- header file for sprite generation program
//

#ifndef SPRITEGN_H
#define SPRITEGN_H

#pragma once

// **********************************************************
// * This file must be identical in the spritegen directory *
// * and in the Quake directory, because it's used to       *
// * pass data from one to the other via .spr files.        *
// **********************************************************

//-------------------------------------------------------
// This program generates .spr sprite package files.
// The format of the files is as follows:
//
// dsprite_t file header structure
// <repeat dsprite_t.numframes times>
//   <if spritegroup, repeat dspritegroup_t.numframes times>
//     dspriteframe_t frame header structure
//     sprite bitmap
//   <else (single sprite frame)>
//     dspriteframe_t frame header structure
//     sprite bitmap
// <endrepeat>
//-------------------------------------------------------

#define SPRITE_VERSION	2

// MAX_SPRITES limit
#define MAX_SPRITES		256

enum synctype_t
{
	ST_SYNC = 0,
	ST_RAND
};

enum spriteframetype_t
{
	SPR_SINGLE = 0,
	SPR_GROUP
};

struct dsprite_t
{
	int					ident;
	int					version;
	spriteframetype_t	type;
	int					texFormat;
	float				boundingradius;
	int					width;
	int					height;
	int					numframes;
	float				beamlength;
	synctype_t			synctype;

	// a word indicating palette color entry size (should be 256)
};

struct dspriteframe_t
{
	int			origin[2];
	int			width;
	int			height;
};

struct dspritegroup_t
{
	int			numframes;
};

struct dspriteinterval_t
{
	float	interval;
};

struct dspriteframetype_t
{
	spriteframetype_t	type;
};

#define IDSPRITEHEADER	(('P'<<24)+('S'<<16)+('D'<<8)+'I') // little-endian "IDSP"

#endif // SPRITEGN_H
