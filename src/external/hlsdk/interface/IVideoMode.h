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
// $NoKeywords: $
//=============================================================================
#if !defined( IVIDEOMODE_H )
#define IVIDEOMODE_H
#pragma once

#define VID_SOFTWARE			0
#define VID_HARDWARE			1
#define VID_HARDWARE_D3D		2

// video mode
struct vmode_t
{
	int			width;
	int			height;
	int			bpp;
};

//-----------------------------------------------------------------------------
// Purpose: Manages the main window's video modes
//-----------------------------------------------------------------------------
class IVideoMode
{
public:
	virtual const char* GetName() = 0;

	virtual bool		Init(void* pvInstance) = 0;
	virtual void		Shutdown() = 0;

	virtual bool		AddMode(int width, int height, int bpp) = 0;
	virtual vmode_t*	GetCurrentMode() = 0;
	virtual vmode_t*	GetMode(int num) = 0;
	virtual int			GetModeCount() = 0;

	virtual bool		IsWindowedMode() const = 0;

	virtual bool		GetInitialized() const = 0;
	virtual void		SetInitialized(bool init) = 0;

	virtual void		UpdateWindowPosition() = 0;

	virtual void		FlipScreen() = 0;

	virtual void		RestoreVideo() = 0;
	virtual void		ReleaseVideo() = 0;
};

extern IVideoMode* videomode;

#endif // IVIDEOMODE_H