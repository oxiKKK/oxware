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
#ifndef COMMAND_H
#define COMMAND_H
#pragma once

// Command dispatch routine
typedef void(__cdecl*xcommand_t)(void);

// Flags for commands
#define FCMD_NONE				0
#define FCMD_HUD				(1 << 0) // HUD command (client dll)
#define FCMD_GAME				(1 << 1) // Server command (server dll)
#define FCMD_WRAPPER			(1 << 2) // GameUI command (GameUI dll)
#define FCMD_FORCE_PRIVILEGED	(1 << 3) // If isn't priviledged by default when executing but this is set, don't allow it.
#define FCMD_FILTERED			(1 << 4) // When cl_filterstuffcmd is on, don't allow.

// Holds information about single command. (_f)
struct cmd_function_t
{
	cmd_function_t*	next;
	char*			name;
	xcommand_t		function;
	int				flags;
};

// Command source - information about where we got the command from.
enum cmd_source_t
{
	src_client,		// came in over a net connection as a clc_stringcmd
					// host_client will be valid during this state.
	src_command		// from the command buffer
};

#define	MAX_ALIAS_NAME 32

// Holds information about user-defined alias.
struct cmdalias_t
{
	cmdalias_t* next;
	char		name[MAX_ALIAS_NAME];
	char*		value;
};

#endif // COMMAND_H
