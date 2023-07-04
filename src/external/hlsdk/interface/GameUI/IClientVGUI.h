//========= Copyright © 1996-2005, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef ICLIENTVGUI_H
#define ICLIENTVGUI_H

#ifdef _WIN32
#pragma once
#endif

#include <interface.h>

//-----------------------------------------------------------------------------
// Purpose: Client VGUI2 interface. Enables the client library to use VGUI2
//-----------------------------------------------------------------------------
class IClientVGUI : public IBaseInterface
{
public:
	/*
	*	Called when this interface is first loaded. Initializes the client UI
	*	Arg1: pFactories Array of factories to use
	*	Arg2: iNumFactories Number of factories
	*/
	virtual void Initialize(CreateInterfaceFn * pFactories, int iNumFactories) = 0;

	/*
	*	Called after the client UI has been initialized and the parent has been set
	*/
	virtual void Start() = 0;

	/*
	*	Sets the parent panel. The root client panel should set this as its parent
	*	Arg1: parent Parent panel
	*/
	virtual void SetParent(vgui2::VPANEL parent) = 0;

	/*
	*	Returns: 1 if VGUI1 should be used, 0 if VGUI2 should be used
	*/
	virtual bool UseVGUI1() = 0;

	/*
	*	Tells the client to hide the score board panel
	*	Called when the game UI is made visible (main menu, etc)
	*/
	virtual void HideScoreBoard() = 0;

	/*
	*	Tells the client to hide all menus, dialogs and windows
	*	Called when the loading progress bar has been stopped
	*/
	virtual void HideAllVGUIMenu() = 0;

	/*
	*	Tells the client to activate its UI
	*	Called when the game UI is hidden
	*/
	virtual void ActivateClientUI() = 0;

	/*
	*	Tells the client to hide its UI
	*	Called when the game UI is activated
	*/
	virtual void HideClientUI() = 0;

	/*
	*	Shuts down the client UI
	*	The engine calls this only for CS & CZero
	*	So client dll calls this directly in HUD_Shutdown() -Enko
	*/
	virtual void Shutdown() = 0;
};

#define CLIENTVGUI_INTERFACE_VERSION "VClientVGUI001"

#endif // ICLIENTVGUI_H