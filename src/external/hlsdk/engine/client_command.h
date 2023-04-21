//======== (C) Copyright 1999, 2000 Valve, L.L.C. All rights reserved. ========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#ifndef CLIENT_COMMAND_H
#define CLIENT_COMMAND_H
#ifdef _WIN32
#pragma once
#endif

// information about movement data packet
typedef struct cmd_s
{
	usercmd_t	cmd;

	float		senttime;
	float		receivedtime;
	float		frame_lerp;

	qboolean	processedfuncs;		// we have ran this command
	qboolean	heldback;

	int			sendsize;
} cmd_t;

#endif // CLIENT_COMMAND_H