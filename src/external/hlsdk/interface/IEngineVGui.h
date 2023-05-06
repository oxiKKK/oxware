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
#if !defined(IENGINEVGUI_H)
#define IENGINEVGUI_H
#ifdef _WIN32
#pragma once
#endif

enum VGUIPANEL
{
	PANEL_ROOT = 0,
	PANEL_CLIENTDLL,
	PANEL_GAMEUIDLL
};

//-----------------------------------------------------------------------------
// Purpose: Centerpoint for handling all user interface in the engine
//-----------------------------------------------------------------------------
class IEngineVGui : public IBaseInterface
{
public:
	virtual					~IEngineVGui() { }

	virtual vgui2::VPANEL	GetPanel(VGUIPANEL type) = 0;

	virtual bool			SteamRefreshLogin(const char* password, bool isSecure) = 0;
	virtual bool			SteamProcessCall(bool* finished, void* progress, void* steamError) = 0;
};

#define ENGINEVGUI_INTERFACE_VERSION "VEngineVGui001"

extern IEngineVGui* enginevgui;

#endif // IENGINEVGUI_H
