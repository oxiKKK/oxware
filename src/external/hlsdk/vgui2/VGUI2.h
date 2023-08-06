//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Basic header for typedef vgui
//
// $NoKeywords: $
//=============================================================================

#ifndef VGUI2_H
#define VGUI2_H

#ifdef _WIN32
	#pragma once
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL    0
	#else
		#define NULL    ((void *)0)
	#endif
#endif

#ifdef WIN32
	#pragma warning( disable: 4355 )	// disables 'this' : used in base member initializer list
#endif

#ifndef _WCHAR_T_DEFINED
	// DAL - wchar_t is a built in define in gcc 3.2 with a size of 4 bytes
	#if !defined( __x86_64__ ) && !defined( __WCHAR_TYPE__  )
		typedef unsigned short wchar_t;
		#define _WCHAR_T_DEFINED
	#endif
#endif

namespace vgui2
{
	// handle to an internal vgui panel
	// this is the only handle to a panel that is valid across dll boundaries
	typedef unsigned int VPANEL;

	// handles to vgui objects
	// NULL_HANDLE values signify an invalid value
	typedef unsigned long HScheme;
	typedef unsigned long HTexture;
	typedef unsigned long HCursor;

	typedef unsigned long HPanel;
	const HPanel INVALID_PANEL = 0xffffffff;

	typedef unsigned long HFont;
	const HFont INVALID_FONT = 0; // the value of an invalid font handle

	#define NULL_HANDLE 0
}

#endif // VGUI2_H
