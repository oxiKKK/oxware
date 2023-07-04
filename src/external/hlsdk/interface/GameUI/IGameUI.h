//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef IGAMEUI_H
#define IGAMEUI_H

#ifdef _WIN32
#pragma once
#endif

// Reasons why the user can't connect to a game server
// TODO: This is a steamcommon.h stuff or something else, add the header file so we can avoid this enum here.
enum ESteamLoginFailure
{
	STEAMLOGINFAILURE_NONE,
	STEAMLOGINFAILURE_BADTICKET,
	STEAMLOGINFAILURE_NOSTEAMLOGIN,
	STEAMLOGINFAILURE_VACBANNED,
	STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE,
	STEAMLOGINFAILURE_CONNECTIONLOST,
	STEAMLOGINFAILURE_NOCONNECTION
};

class IBaseSystem;

//-----------------------------------------------------------------------------
// Purpose: GameUI Interface
//-----------------------------------------------------------------------------
class IGameUI : public IBaseInterface
{
public:
	/*
	*	Initializes GameUI and vgui2 interfaces
	*	Called by the engine in CBaseUI::Initialize()
	*/
	virtual void Initialize(CreateInterfaceFn* factories, int count) = 0;

	/*
	*	Sets up the GameUI
	*	Called by the engine in CBaseUI::Initialize(), after IGameUI::Initialize()
	*/
	virtual void Start(cl_enginefunc_t* engineFuncs, int interfaceVersion, IBaseSystem* system) = 0;

	/*
	*	Shutdowns GameUI
	*	Called by the engine in CBaseUI::Shutdown()
	*/
	virtual void Shutdown(void) = 0;

	/*
	*	Just wraps an engine call to activate the GameUI
	*/
	virtual int ActivateGameUI(void) = 0;

	/*
	*	Activates DemoUI
	*/
	virtual int ActivateDemoUI(void) = 0;

	/*
	*	Returns true on the game UI having exclusive input, false otherwise
	*	Literally IsGameUIActive() != 0
	*/
	virtual int HasExclusiveInput(void) = 0;

	/*
	*	Paints all the vgui2 elements
	*	Called every frame by the engine in CBaseUI::Paint()
	*/
	virtual void RunFrame(void) = 0;

	/*
	*	Called when the game connects to a server
	*	If this called when not playing a Career Match, calls HideGameUI()
	*	and stops mp3 track, if any
	*/
	virtual void ConnectToServer(const char* game, int IP, int port) = 0;

	/*
	*	Called when the game disconnects from a server
	*	Calls ActivateGameUI()
	*/
	virtual void DisconnectFromServer(void) = 0;

	/*
	*	Just wraps an engine call to hide the GameUI
	*	The GameUI module calls this in ConnectToServer() and many elsewhere
	*	Use this function to hide the GameUI
	*/
	virtual void HideGameUI(void) = 0;

	/*
	*	Returns true whether the GameUI active
	*/
	virtual bool IsGameUIActive(void) = 0;

	/*
	*	Activates the loading dialog on level load start
	*	Called by the engine when connecting/starting a map, etñ
	*/
	virtual void LoadingStarted(const char* resourceType, const char* resourceName) = 0;

	/*
	*	Closes any level load dialog
	*	Called by the engine when the loading dialog must be aborted
	*/
	virtual void LoadingFinished(const char* resourceType, const char* resourceName) = 0;

	/*
	*	Starts progress bar, sets progress ticks
	*	Called by the engine manywhere
	*/
	virtual void StartProgressBar(const char* progressType, int progressSteps) = 0;

	/*
	*	Returns true if the screen should be updated
	*/
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) = 0;

	/*
	*	Stops progress bar, displays error if necessary
	*/
	virtual void StopProgressBar(bool bError, const char* failureReason, const char* extendedReason = nullptr) = 0;

	/*
	*	Sets loading info text
	*/
	virtual int SetProgressBarStatusText(const char* statusText) = 0;

	/*
	*	Sets secondary progress bar
	*	Called by the engine when downloading resources from server
	*	NOTE: The value mast be in range [0..1]
	*/
	virtual void SetSecondaryProgressBar(float progress) = 0;

	/*
	*	Sets loading info text for secondary bar
	*	Called by the engine when downloading resources from server
	*/
	virtual void SetSecondaryProgressBarText(const char* statusText) = 0;

	/*
	*	Supposed to validate the user has a cdkey in the registry
	*	Does nothing
	*/
	virtual void ValidateCDKey(bool force, bool inConnect) = 0;

	/*
	*	Called when the client disconnects from a server
	*/
	virtual void OnDisconnectFromServer(ESteamLoginFailure eSteamLoginFailure, const char* username) = 0;
};

extern IGameUI* g_pGameUI;

#define GAMEUI_INTERFACE_VERSION "GameUI007"

#endif // IGAMEUI_H