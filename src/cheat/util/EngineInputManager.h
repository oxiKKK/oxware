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

#ifndef ENGINEINPUTMANAGER_H
#define ENGINEINPUTMANAGER_H
#pragma once

class CEngineInput
{
public:
	DECL_BASIC_CLASS(CEngineInput);

public:
	void initialize();

	// SDL2 routines. Note that these are only relevant on SDL engine builds.
	void(__cdecl*SDL_WarpMouseInWindow)(void* window, int x, int y);
	int(__cdecl*SDL_SetRelativeMouseMode)(int enabled);
	unsigned(__cdecl* SDL_GetRelativeMouseState)(int* x, int* y);
	void(__cdecl*SDL_PumpEvents)();
	int(__cdecl*SDL_GetRelativeMouseMode)();

	// main vgui2 panels
	hl::vgui2::VPANEL vgui_static_panel() const;
	hl::vgui2::VPANEL vgui_client_panel() const;
	hl::vgui2::VPANEL vgui_gameui_panel() const;

	// client dll input
	void toggle_ingame_clientdll_mouse(bool active);
	bool is_ingame_clientdll_mouse_enabled() const { return m_clientdll_mouse_enabled; }

	// BaseUI_Surface engine surface code
	void surface_disable_cursor();
	void surface_enable_cursor();
	void surface_set_cursor(hl::vgui2::CursorCode code);
	void surface_set_cursor_pos(const Vector2D& pos);
	void surface_centerize_cursor_position();

	// miscellaneous handling
	// enables/disabled ingame input depending on several scenarious
	void toggle_ingame_input(bool enable);

	// relative mouse mode (depending on m_rawinput)
	void toggle_relative_mouse_mode(bool enable);

	bool is_in_relative_mode(); // gameui is not rendering and we're ingame
	bool is_gameui_rendering();

	void toggle_windows_mouse_cursor(bool show);

private:
	bool m_clientdll_mouse_enabled = false;

	HCURSOR m_last_cursor_handle = NULL;

	HCURSOR get_current_cursor_handle();
};

#endif // ENGINEINPUTMANAGER_H