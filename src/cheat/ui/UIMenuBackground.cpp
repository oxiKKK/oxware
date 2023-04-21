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

void CUIMenuBackground::on_initialize()
{
	m_fade_density = 0.0f;
}

void CUIMenuBackground::on_render()
{
	update_density();

	// can happen that this gets called before the interface hook is initialized. oh well
	auto gameuifuncs = CHLInterfaceHook::the().IGameUI();
	if (gameuifuncs && gameuifuncs->IsGameUIActive() && CGameUtil::the().is_fully_connected())
	{
		// when the GameUI is up, black box of opacity 128 is rendered on the screen. Don't blindly render our box over it.
		m_fade_density -= 128.0f / 255.0f;
	}

	g_gui_window_rendering_i->render_box(g_gui_window_rendering_i->get_background_drawlist(), 
										 { 0.0f, 0.0f }, g_imgui_platform_layer_i->get_screen_size(), {0.0f, 0.0f, 0.0f, m_fade_density});
}

void CUIMenuBackground::on_destroy()
{
}

void CUIMenuBackground::update_density()
{
	m_fade_density = k_max_fade_density;

#if 0
	if (m_fade_density < k_max_fade_density)
	{
		m_fade_density += 0.01f;
	}
#endif
}
