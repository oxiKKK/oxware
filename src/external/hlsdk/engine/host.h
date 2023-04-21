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

#if !defined( HOST_H )
#define HOST_H
#pragma once

//=============================================================================
// the host system specifies the base of the directory tree, the
// command line parms passed to the program, and the amount of memory
// available for the program to use
struct quakeparms_t
{
	char*	basedir;		// Base / Mod game directory
	char*	cachedir;		// for development over ISDN lines

	int		argc;
	char**	argv;

	void*	membase;
	int		memsize;
};

#endif // HOST_H