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
// sound.h -- Half-Life's engine sound intrerface
//

#ifndef SOUND_H
#define SOUND_H
#pragma once

// Miles sound system
#define MAX_MSS_FREQ			22050
#define MAX_MSS_BITS			16
#define MAX_MSS_CHANNELS		2

// Load limit for COM_LoadFileLimit() used when loading stream sound.
#define STREAMSND_FILE_LIMIT	0x8000 // 32,768 bytes

#define PAINTBUFFER_SIZE		512

// Maximum size of a wavstream chunk when calling COM_LoadFileLimit().
// Same as MAX_DEMOFILE_LENGTH.
#define WAVSTREAM_BIG_CHUNK		0x8000

// sound engine rate defines
#define SOUND_DMA_SPEED			MAX_MSS_FREQ // hardware playback rate
#define SOUND_DMA_SAMPLES		0x8000
#define SOUND_DMA_SAMPLE_BITS	16			 // bits per sample

// Sample rate of a playing sound
#define SOUND_11k				(SOUND_DMA_SPEED / 2)	// 11khz sample rate

// When caching a sound, a.k.a. sfx, for voice, the length's 30th bit
//			is set only, determining that we're caching a voice sound.
#define SND_VOICE_DETERMINANT	(1 << 30) // 0x40000000

// Pre-computed sound scale table
#define SND_SCALETABLE_HEIGHT	32
#define SND_SCALETABLE_WIDTH	256

// Maximum MAX_CHANNEL_VOLUME on a channel (left, right / rear right, rear left)
#define MAX_CHANNEL_VOLUME		255.0f

// For buffer locking attempts. Used inside internal engine sound code.
#define MAX_LOCK_ATTEMPTS		10000

#define CVOXWORDMAX				32

// sentence groups
#define CBSENTENCENAME_MAX		16
#define CVOXFILESENTENCEMAX		1536 // max number of sentences in game. NOTE: this must match
									 // CVOXFILESENTENCEMAX in dlls\util.h!!!

//
// FX presets
//

#define SXROOM_OFF				0	

#define SXROOM_GENERIC			1	// general, low reflective, diffuse room

#define SXROOM_METALIC_S		2	// highly reflective, parallel surfaces
#define SXROOM_METALIC_M		3
#define SXROOM_METALIC_L		4

#define SXROOM_TUNNEL_S			5	// resonant reflective, long surfaces
#define SXROOM_TUNNEL_M			6
#define SXROOM_TUNNEL_L			7

#define SXROOM_CHAMBER_S		8	// diffuse, moderately reflective surfaces
#define SXROOM_CHAMBER_M		9
#define SXROOM_CHAMBER_L		10

#define SXROOM_BRITE_S			11	// diffuse, highly reflective
#define SXROOM_BRITE_M			12
#define SXROOM_BRITE_L			13

#define SXROOM_WATER1			14	// underwater fx
#define SXROOM_WATER2			15
#define SXROOM_WATER3			16

#define SXROOM_CONCRETE_S		17	// bare, reflective, parallel surfaces
#define SXROOM_CONCRETE_M		18
#define SXROOM_CONCRETE_L		19

#define SXROOM_OUTSIDE1			20	// echoing, moderately reflective
#define SXROOM_OUTSIDE2			21	// echoing, dull
#define SXROOM_OUTSIDE3			22	// echoing, very dull

#define SXROOM_CAVERN_S			23	// large, echoing area
#define SXROOM_CAVERN_M			24
#define SXROOM_CAVERN_L			25

#define SXROOM_WEIRDO1			26	
#define SXROOM_WEIRDO2			27
#define SXROOM_WEIRDO3			28

#define CSXROOM					29

// soundeffect object
typedef struct
{
	char 			name[MAX_QPATH]; // ?VoiceData when voice
	cache_user_t	cache;
	int				servercount;
} sfx_t;

// Flags for S_Play Dynamic/Static Sound functions
#define SND_NONE			0x00
#define SND_SENTENCE		0x10  // set if sound num is actually a sentence num
#define SND_STOP			0x20  // duplicated in dlls/util.h stop sound
#define SND_CHANGE_VOL		0x40  // duplicated in dlls/util.h change sound vol
#define SND_CHANGE_PITCH	0x80  // duplicated in dlls/util.h change sound pitch
#define SND_SPAWNING		0x100 // duplicated in dlls/util.h we're spawing, used in some cases for ambients 

// sfx cache
struct sfxcache_t
{
	int 			length;
	int 			loopstart;
	int 			speed;
	int 			width;
	int 			stereo;
	byte			data[1];		// variable sized
};

// Dynamic memory access block
struct dma_t
{
	qboolean		gamealive;		// TRUE when the dma buffer is present & initialized.
	qboolean		soundalive;
	qboolean		splitbuffer;

	int				channels;
	int				samples;
	int				submission_chunk;
	int				samplepos;
	int				samplebits;

	int				speed;
	int				dmaspeed;

	byte*			buffer;
};

// information about single sound channel
typedef struct
{
	sfx_t*			sfx;			// sfx number

	int				leftvol;		// 0-255 volume
	int				rightvol;		// 0-255 volume

	int				end;			// end time in global paintsamples
	int				pos;			// sample position in sfx

	int				looping;		// where to loop, -1 = no looping

	int				entnum;			// to allow overriding a specific sound
	int				entchannel;		// TODO: Define as enum, modify const.h then -Enko

	vec3_t			origin;			// origin of sound effect
	vec_t			dist_mult;		// distance multiplier (attenuation/clipK)

	int				master_vol;		// 0-255 master volume

	int				isentence;
	int				iword;

	int				pitch;

#if defined(_ADD_EAX_)
	int				rleftvol;		// 0-255 volume
	int				rrightvol;		// 0-255 volume
#endif
} channel_t;

// WAVe format internal data
typedef struct
{
	int				rate;
	int				width;
	int				channels;
	int				loopstart;
	int				samples;
	int				dataofs;
} wavinfo_t;

// WAVe file object
typedef struct
{
	int				csamplesplayed;
	int				csamplesinmem;
	FileHandle_t	hFile;
	wavinfo_t		info;
	int				lastposloaded;
} wavstream_t;

// VOX word
typedef struct
{
	int		volume;					// increase percent, ie: 125 = 125% increase
	int		pitch;					// pitch shift up percent
	int		start;					// offset start of wave percent
	int		end;					// offset end of wave percent
	int		cbtrim;					// end of wave after being trimmed to 'end'
	int		fKeepCached;			// 1 if this word was already in cache before sentence referenced it
	int		samplefrac;				// if pitch shifting, this is position into wav * 256
	int		timecompress;			// % of wave to skip during playback (causes no pitch shift)
	sfx_t*	sfx;					// name and cache pointer
} voxword_t;

//=============================================================================

#define	MAX_CHANNELS			128
#define	MAX_DYNAMIC_CHANNELS	8

#endif // SOUND_H
