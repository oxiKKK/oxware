//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

//
// userid.h -- UserIDdata for networking and user banning
//

#ifndef USER_H
#define USER_H
#pragma once

// Authentication types for idtype
#define IDTYPE_WON		0
#define IDTYPE_STEAM	1
#define IDTYPE_VALVE	2
#define IDTYPE_HLTV		3

// UserID Data
typedef struct
{
	int idtype;				// Auth type
	uint64 m_SteamID;		// Client's Steam ID

	unsigned int clientip;	// Client's IP Address
} USERID_t;

#endif // USER_H