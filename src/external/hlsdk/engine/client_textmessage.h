//====== Copyright © 1996-2003, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef CLIENT_TEXTMESSAGE_H
#define CLIENT_TEXTMESSAGE_H
#ifdef _WIN32
#pragma once
#endif

// client-side screen text message data
typedef struct client_textmessage_s
{
	int			effect;				// effect number
	byte		r1, g1, b1, a1;		// 2 colors for effects
	byte		r2, g2, b2, a2;
	float		x, y;				// position
	float		fadein, fadeout;	// fade
	float		holdtime;
	float		fxtime;
	const char *pName;
	const char *pMessage;
} client_textmessage_t;

#endif // CLIENT_TEXTMESSAGE_H
