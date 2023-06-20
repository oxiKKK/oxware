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

void CUIBackgroundRendering::on_initialize()
{
}

void CUIBackgroundRendering::on_render()
{
	if (!CGameUtil::the().is_fully_connected())
	{
		return;
	}

	// can happen that this gets called before the interface hook is initialized. oh well
	auto gameuifuncs = CHLInterfaceHook::the().IGameUI();
	if (gameuifuncs && gameuifuncs->IsGameUIActive())
	{
		return;
	}

	auto screen_pos = CVideoModeUtil::the().get_ingame_viewport_pos();
	auto screen_size = CVideoModeUtil::the().get_real_screen_size();

	if (screen_size.IsZero())
	{
		return;
	}

	g_gui_widgets_i->set_next_window_pos({ screen_pos.x, 0 }, ImGuiCond_Always);
	g_gui_widgets_i->set_next_window_size(screen_size, ImGuiCond_Always);

	static constexpr auto window_flags =
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNav;

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	g_gui_widgets_i->create_new_window(
		"background_rendering", window_flags, [&]()
		{
#if 0 // debug: render a rect around bounds of this window
			g_gui_window_rendering_i->render_box_outline(
				g_gui_window_rendering_i->get_current_drawlist(), 
				screen_pos, screen_pos + screen_size,
				CColor(230, 0, 0, 0), 0, 
				CColor(230, 0, 0, 230), 4.0f
			);
#endif

			for (auto& callback : m_render_code_callbacks)
			{
				if (callback)
					callback();
			}
		});

	g_gui_widgets_i->pop_stylevar(1);
}

void CUIBackgroundRendering::on_destroy()
{
}
