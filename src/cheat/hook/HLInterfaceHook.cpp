/*
*	OXWARE developed by oxiKKK
*	Copyright (c) 2023
* 
*	This program is licensed under the MIT license. By downloading, copying, 
*	installing or using this software you agree to this license.
*
*	License Agreement
*
*	Permission is hereby granted, free of charge, to any person obtaining a 
*	copy of this software and associated documentation files (the "Software"), 
*	to deal in the Software without restriction, including without limitation 
*	the rights to use, copy, modify, merge, publish, distribute, sublicense, 
*	and/or sell copies of the Software, and to permit persons to whom the 
*	Software is furnished to do so, subject to the following conditions:
*
*	The above copyright notice and this permission notice shall be included 
*	in all copies or substantial portions of the Software. 
*
*	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
*	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
*	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
*	THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
*	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
*	IN THE SOFTWARE.
*/

#include "precompiled.h"

// we have to instantiate this here and set its pointer later because some of the effect
// constructors use this global variable.
namespace hl
{
hl::IParticleMan* g_pParticleMan = nullptr;
}

bool CHLInterfaceHook::install_hooks()
{
	// install individual hooks
	if (!install_single_hook(&m_ISurface, L"hw.dll", VGUI_SURFACE_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IPanel, L"vgui2.dll", VGUI_PANEL_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_ISchemeManager, L"vgui2.dll", VGUI_SCHEME_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IEngineVGui, L"hw.dll", ENGINEVGUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IGameUI, L"GameUI.dll", GAMEUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IBaseUI, L"hw.dll", BASEUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IClientVGUI, L"client.dll", CLIENTVGUI_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IFileSystem, COxWare::the().get_engine_fs_module_name(), FILESYSTEM_INTERFACE_VERSION)) return false;
	if (!install_single_hook(&m_IParticleMan, L"particleman.dll", PARTICLEMAN_INTERFACE_VERSION)) return false;

	// steam
	if (!install_steam_interfaces())
	{
		return false;
	}

	// set it for effects interface to function properly
	hl::g_pParticleMan = m_IParticleMan;

	return true;
}

bool CHLInterfaceHook::install_steam_interfaces()
{
	if (COxWare::the().get_build_number() < FIRST_SDL_BUILD)
	{
		return true; // TODO: Do we need this on such builds?
	}

	if (!install_single_hook(&m_ISteamClient, L"steamclient.dll", STEAMCLIENT_INTERFACE_VERSION)) return false;

	auto hSteamPipe = m_ISteamClient->CreateSteamPipe();
	auto hSteamUser = m_ISteamClient->ConnectToGlobalUser(hSteamPipe);

	m_ISteamFriends = m_ISteamClient->GetISteamFriends(hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION);
	m_ISteamUtils = m_ISteamClient->GetISteamUtils(hSteamUser, STEAMUTILS_INTERFACE_VERSION);
	m_ISteamUser = m_ISteamClient->GetISteamUser(hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION);

	return true;
}
