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
// netbuf.h -- A buffer to be transfered across the network.
//

#ifndef NETBUF_H
#define NETBUF_H
#pragma once

// For flags of sizebuf_t
enum FSB
{
	FSB_NONE = 0,

	// If not set, do a Sys_Error
	FSB_ALLOWOVERFLOW = 1 << 0,

	// Set if the buffer size failed
	FSB_OVERFLOWED = 1 << 1
};

// oject representing single package of buffer data
struct sizebuf_t
{
	const char*		buffername;

	unsigned short	flags;

	byte*			data;

	int				maxsize;
	int				cursize;
};

// buffer to be written to
struct bf_write_t
{
	int				nCurOutputBit;
	unsigned char*	pOutByte;

	sizebuf_t*		pbuf;
};

// buffer to be read from
struct bf_read_t
{
	int				msg_readcount;
	sizebuf_t*		pbuf;

	int				nBitFieldReadStartByte;
	int				nBytesRead;
	int				nCurInputBit;

	unsigned char*	pInByte;
};

#endif // NETBUF_H