//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

//
// usermsg.h -- A type of message to be transfered via parsing code.
//

#ifndef USERMSG_H
#define USERMSG_H
#ifdef _WIN32
#pragma once
#endif

typedef int(*pfnUserMsgHook)(const char* pszName, int iSize, void* pbuf);

// Circular linked list containing user message data for each command.
struct UserMsg
{
	// This is the usermsg number
	int				iMsg;

	// Maximum size is MAX_USERMSG_SIZE
	int				iSize;

	// Name of the message
	char			szName[MAX_USERMSG_NAME];

	// Next entry in the list. If this is head, that points to tail.
	UserMsg*		next;

	// Routine that will be dispatched
	pfnUserMsgHook	pfn;
};

#endif // USERMSG_H