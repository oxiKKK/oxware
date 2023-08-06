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

#ifndef HLINTERFACEHOOK_H
#define HLINTERFACEHOOK_H
#pragma once

class CHLInterfaceHook
{
public:
	DECL_BASIC_CLASS(CHLInterfaceHook);

public:
	bool install_hooks();

	inline auto ISurface() const { return m_ISurface; }
	inline auto IPanel() const { return m_IPanel; }
	inline auto ISchemeManager() const { return m_ISchemeManager; }
	inline auto IEngineVGui() const { return m_IEngineVGui; }
	inline auto IGameUI() const { return m_IGameUI; }
	inline auto IBaseUI() const { return m_IBaseUI; }
	inline auto IClientVGUI() const { return m_IClientVGUI; }
	inline auto IFileSystem() const { return m_IFileSystem; }
	inline auto IParticleMan() const { return m_IParticleMan; }

	// steam
	inline auto ISteamClient() const { return m_ISteamClient; }
	inline auto ISteamFriends() const { return m_ISteamFriends; }
	inline auto ISteamUtils() const { return m_ISteamUtils; }
	inline auto ISteamUser() const { return m_ISteamUser; }

private:
	template<typename T>
	bool install_single_hook(T** hook, const wchar_t* dll_name, const char* interface_name);

	bool install_steam_interfaces();

private:
	hl::vgui2::ISurface* m_ISurface;
	hl::vgui2::IPanel* m_IPanel;
	hl::vgui2::ISchemeManager* m_ISchemeManager;
	hl::IEngineVGui* m_IEngineVGui;
	hl::IGameUI* m_IGameUI;
	hl::IBaseUI* m_IBaseUI;
	hl::IClientVGUI* m_IClientVGUI;
	hl::IFileSystem* m_IFileSystem;
	hl::IParticleMan* m_IParticleMan;

	// steam
	hl::ISteamClient* m_ISteamClient;
	hl::ISteamFriends* m_ISteamFriends;
	hl::ISteamUtils* m_ISteamUtils;
	hl::ISteamUser* m_ISteamUser;
};

template<typename T>
inline bool CHLInterfaceHook::install_single_hook(T** hook, const wchar_t* dll_name, const char* interface_name)
{
	auto factory = hl::Sys_GetFactory((HMODULE)g_libloader_i->get_target_loaded_dll_base_address(dll_name));
	if (factory == nullptr)
	{
		CMessageBox::display_error("Couldn't find HL interface factory inside module '{}'", CStringTools::the().utf16_to_utf8(dll_name));
		return false;
	}

	*hook = reinterpret_cast<T*>(factory(interface_name, NULL));
	if (*hook == nullptr)
	{
		CMessageBox::display_error("Couldn't find HL interface '{}' pointer inside module '{}'", interface_name, CStringTools::the().utf16_to_utf8(dll_name));
		return false;
	}

	CConsole::the().info("Found {} interface in {}.", interface_name, CStringTools::the().utf16_to_utf8(dll_name));

	return true;
}

#endif // HLINTERFACEHOOK_H