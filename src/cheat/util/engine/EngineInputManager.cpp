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

void CEngineInput::initialize()
{
	if (COxWare::the().get_build_number() >= FIRST_SDL_BUILD)
	{
		SDL_WarpMouseInWindow = (decltype(SDL_WarpMouseInWindow))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_WarpMouseInWindow");
		SDL_SetRelativeMouseMode = (decltype(SDL_SetRelativeMouseMode))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_SetRelativeMouseMode");
		SDL_GetRelativeMouseState = (decltype(SDL_GetRelativeMouseState))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_GetRelativeMouseState");
		SDL_PumpEvents = (decltype(SDL_PumpEvents))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_PumpEvents");
		SDL_GetRelativeMouseMode = (decltype(SDL_GetRelativeMouseMode))GetProcAddress(GetModuleHandleA("SDL2.dll"), "SDL_GetRelativeMouseMode");
	}

	CConsole::the().info("Engine input initialized.");
}

hl::vgui2::VPANEL CEngineInput::vgui_static_panel() const
{
	auto enginevgui = CHLInterfaceHook::the().IEngineVGui();
	if (enginevgui)
	{
		return enginevgui->GetPanel(hl::PANEL_ROOT);
	}
	return NULL_HANDLE;
}

hl::vgui2::VPANEL CEngineInput::vgui_client_panel() const
{
	auto enginevgui = CHLInterfaceHook::the().IEngineVGui();
	if (enginevgui)
	{
		return (hl::vgui2::VPANEL)enginevgui->GetPanel(hl::PANEL_CLIENTDLL);
	}
	return NULL_HANDLE;
}

hl::vgui2::VPANEL CEngineInput::vgui_gameui_panel() const
{
	auto enginevgui = CHLInterfaceHook::the().IEngineVGui();
	if (enginevgui)
	{
		return (hl::vgui2::VPANEL)enginevgui->GetPanel(hl::PANEL_GAMEUIDLL);
	}
	return NULL_HANDLE;
}

void CEngineInput::toggle_ingame_clientdll_mouse(bool active)
{
	m_clientdll_mouse_enabled = active;

	auto cl_funcs = CMemoryHookMgr::the().cl_funcs();
	if (cl_funcs)
	{
		if (active)
		{
			cl_funcs->pfnHUD_IN_ActivateMouse();
		}
		else
		{
			cl_funcs->pfnHUD_IN_DeactivateMouse();
		}
	}
}

void CEngineInput::surface_disable_cursor()
{
	surface_set_cursor(hl::vgui2::dc_none);
}

void CEngineInput::surface_enable_cursor()
{
	surface_set_cursor(hl::vgui2::dc_arrow);
}

void CEngineInput::surface_set_cursor(hl::vgui2::CursorCode code)
{
	auto surfacefuncs = CHLInterfaceHook::the().ISurface();
	if (surfacefuncs)
	{
		surfacefuncs->UnlockCursor();
		surfacefuncs->SetCursor(code);
		surfacefuncs->LockCursor();
	}
}

void CEngineInput::surface_set_cursor_pos(const Vector2D& pos)
{
	Vector2D pos_to_set = pos;

	if (COxWare::the().get_build_number() >= FIRST_SDL_BUILD)
	{
		auto pmainwindow = *CMemoryHookMgr::the().pmainwindow().get();
		if (!*pmainwindow)
		{
			return;
		}

		SDL_WarpMouseInWindow(*pmainwindow, (int)pos_to_set.x, (int)pos_to_set.y);

		int x, y;
		SDL_PumpEvents();
		SDL_GetRelativeMouseState(&x, &y);
	}
	else
	{
		// This is in absolute coordinates, so offset from the window position
		pos_to_set += CGameUtil::the().get_window_pos();
		SetCursorPos((int)pos_to_set.x, (int)pos_to_set.y);
	}
}

void CEngineInput::surface_centerize_cursor_position()
{
	auto offset = CVideoModeUtil::the().get_ingame_viewport_pos();
	surface_set_cursor_pos(offset + CVideoModeUtil::the().get_real_screen_size() / 2.0f);
}

void CEngineInput::toggle_ingame_input(bool enable)
{
	// panels
	auto client_panel = vgui_client_panel();
	auto gameui_panel = vgui_gameui_panel();

	// interfaces
	auto ipanel = CHLInterfaceHook::the().IPanel();
	auto igameui = CHLInterfaceHook::the().IGameUI();
	auto iclientvgui = CHLInterfaceHook::the().IClientVGUI();

	if ((!client_panel || !gameui_panel) || // sanity checks
		(!ipanel || !igameui || !iclientvgui))
	{
		CConsole::the().error("Couldn't handle engine input because some of the hooks were null!");
		return;
	}

	//
	// Handle ingame inputs. 
	// 
	// If we decide to enable ingame inputs, we'll re-enable all engine input states
	// so that the engine thinks that we want to capture input from it. Otherwise, we
	// disable all possible engine inputs, so that the engine thinks that we're competely
	// numb in terms of inputs.
	// 
	// Note for the relative mouse mode: Call the function always after engine functions, 
	// because we as the modifier want to have the last "word", always.
	//

	if (enable)
	{
		// handle inputs when deactivating the cheat
		if (is_in_relative_mode())
		{
			ipanel->SetVisible(client_panel, true);
			ipanel->SetMouseInputEnabled(client_panel, true);
			iclientvgui->ActivateClientUI();
			toggle_relative_mouse_mode(true);
			toggle_windows_mouse_cursor(false);
		}
		else
		{
			toggle_windows_mouse_cursor(true);
			surface_enable_cursor();
			toggle_relative_mouse_mode(false);
		}
	}
	else
	{
		// handle inputs when deactivating the cheat
		if (is_in_relative_mode())
		{
			ipanel->SetVisible(client_panel, false);
			ipanel->SetMouseInputEnabled(client_panel, false);
			iclientvgui->HideClientUI();
		}

		toggle_windows_mouse_cursor(false);
		surface_disable_cursor();

		toggle_relative_mouse_mode(false);
	}
}

void CEngineInput::toggle_relative_mouse_mode(bool enable)
{
	if (COxWare::the().get_build_number() < FIRST_SDL_BUILD)
	{
		return; // this isn't a thing on older builds.
	}

	// Relative mouse mode without m_rawinput can cause issues. This is by the engine design.
	auto m_rawinput = CGoldSrcCommandMgr::the().get_cvar("m_rawinput");
	if (m_rawinput && m_rawinput->value != 0.0f)
	{
		SDL_SetRelativeMouseMode(enable);
	}
}

bool CEngineInput::is_in_relative_mode()
{
	return !is_gameui_rendering() && CGameUtil::the().is_fully_connected();
}

bool CEngineInput::is_gameui_rendering()
{
	auto igameui = CHLInterfaceHook::the().IGameUI();
	if (!igameui)
	{
		return false;
	}
	return igameui->IsGameUIActive();
}

void CEngineInput::toggle_windows_mouse_cursor(bool show)
{
	return;
	// assuming we have set ImGuiConfigFlags_NoMouseCursorChange in imgui platform layer, we handle
	// cursor showing/hiding for ourselfs.

	if (show)
	{
		assert(m_last_cursor_handle != nullptr && "called " __FUNCTION__ " with already shown cursor!");

		SetCursor(m_last_cursor_handle);
		m_last_cursor_handle = NULL;
	}
	else
	{
		assert(m_last_cursor_handle == nullptr && "called " __FUNCTION__ " with already hiddne cursor!");

		// set before hidden
		m_last_cursor_handle = get_current_cursor_handle();
		CConsole::the().info("cursor handle: 0x{:08X}", (DWORD)m_last_cursor_handle);
		SetCursor(NULL);
	}
}

HCURSOR CEngineInput::get_current_cursor_handle()
{
	CURSORINFO ci;
	ci.cbSize = sizeof(CURSORINFO);
	BOOL success = GetCursorInfo(&ci);
	if (!success)
	{
		CConsole::the().error("Failed to get cursor info. (last error: {})", CGenericUtil::the().get_last_win_error());
		return NULL;
	}

	return ci.hCursor;
}
