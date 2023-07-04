//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

//
// wad.h -- Where's all the data
//

#ifndef WAD_H
#define WAD_H
#pragma once

#define	CMP_NONE		0
#define	CMP_LZSS		1

#define	TYP_NONE		0
#define	TYP_LABEL		1

#define	TYP_LUMPY		64 // 64 + grab command number
#define	TYP_PALETTE		64
#define	TYP_QTEX		65
#define	TYP_QPIC		66
#define	TYP_SOUND		67
#define	TYP_MIPTEX		68

// Two preserved indexes inside the wasd list ofr gfx and fonts.
#define WAD_IDX_GFX		0 // used to store the "loading" logo for example
#define WAD_IDX_FONTS	1

typedef struct
{
	int			width, height;
	byte		data[4];		// variably sized
} qpic_t;

// General information about this wad file. This is used as a header.
typedef struct
{
	char		identification[4];	// should be WAD3 or 3DAW
	int			numlumps;			// Number of lumps inside this wad file
	int			infotableofs;		// offset into lumpinfo_t data
} wadinfo_t;

#define WAD_MAX_NAME 16

typedef struct
{
	int			filepos;
	int			disksize;				// Size of the file in  bytes
	int			size;					// uncompressed size
	char		type;
	char		compression;
	char		pad1, pad2;
	char		name[WAD_MAX_NAME];		// must be null terminated
} lumpinfo_t;

#define WADLIST_MAX_NAME 32

typedef struct
{
	bool		loaded;
	char		wadname[WADLIST_MAX_NAME];
	int			wad_numlumps;
	lumpinfo_t* wad_lumps;
	byte*		wad_base;
} wadlist_t;

int		W_LoadWadFile(char* filename);
void	W_CleanupName(char* in, char* out);
lumpinfo_t* W_GetLumpinfo(int wad, char* name, qboolean doerror);
void*	W_GetLumpName(int wad, char* name);
void*	W_GetLumpNum(int wad, int num);
void	W_Shutdown();

void	SwapPic(qpic_t* pic);

#endif // WAD_H
