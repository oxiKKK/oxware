//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IBASEUI_H
#define IBASEUI_H

#pragma once

//-----------------------------------------------------------------------------
// Purpose: Contains all the functions that the BaseUI code exports
//-----------------------------------------------------------------------------
class IBaseUI : public IBaseInterface
{
public:
	/*
	*	Called when this interface is first loaded. Initializes the game UI
	*	Arg1: pFactories Array of factories to use
	*	Arg2: iNumFactories Number of factories
	*/
	virtual void Initialize( CreateInterfaceFn* pFactories, int iNumFactories ) = 0;

	/*
	*	Sets up the game UI
	*/
	virtual void Start( cl_enginefunc_t* pEngfuncs, int iVersion ) = 0;

	/*
	*	Shuts down the UI. Unloads libraries used by the UI
	*/
	virtual void Shutdown( void ) = 0;

	/*
	*	Called when a key event occurs
	*	Arg1: down Whether the key was up or down
	*	Arg2: keynum Key number that was pressed or released
	*	Arg3: pszCurrentBinding The current binding for the key
	*	Returns: Whether the event was handled
	*/
	virtual int Key_Event( int down, int keynum, const char* pszCurrentBinding ) = 0;

	/*
	*	Calls the engine's surface app handler to handle the given SDL event
	*	Arg1: pEvent Event
	*	Arg2: pUserData User data associated with the event. Ignored
	*/
	virtual void CallEngineSurfaceAppHandler( void* pEvent, void* pUserData ) = 0;

	/*
	*	Draw the section of the VGUI2 UI that is contained in the given rectangle
	*/
	virtual void Paint( int x, int y, int right, int bottom ) = 0;

	/*
	*	Hides the game UI and activates the client UI
	*/
	virtual void HideGameUI( void ) = 0;

	/*
	*	Activates the game UI and hides the client UI
	*/
	virtual void ActivateGameUI( void ) = 0;

	/*
	*	Hides the game console
	*/
	virtual void HideConsole( void ) = 0;

	/*
	*	Shows the game console
	*/
	virtual void ShowConsole( void ) = 0;
};

#define BASEUI_INTERFACE_VERSION "BaseUI001"

#endif // IBASEUI_H
