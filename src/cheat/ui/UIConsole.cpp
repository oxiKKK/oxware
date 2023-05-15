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

void CUIConsole::on_initialize()
{
}

void CUIConsole::on_render()
{
	g_gui_widgets_i->set_next_window_size({ 700.0f, 400.0f }, ImGuiCond_Once);
	g_gui_widgets_i->set_next_window_rounding(4.0f, ImDrawFlags_RoundCornersAll);

	static constexpr auto window_flags = 
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings | 
		ImGuiWindowFlags_NoNav;

	g_gui_widgets_i->push_stylevar(ImGuiStyleVar_WindowPadding, { 8.0f, 2.0f });

	g_gui_widgets_i->create_new_window(
		"console", window_flags, [&]()
		{
			g_gui_widgets_i->add_console();
		});

	g_gui_widgets_i->pop_stylevar(); // padding
}

void CUIConsole::on_destroy()
{
}
