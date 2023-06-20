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

void CVideoModeUtil::initialize()
{
	// following information gets changed only on game restart.

	auto ivideomode = *CMemoryHookMgr::the().IVideoMode().get();

	auto mode = ivideomode->GetCurrentMode();
	assert(mode && "This shouldn't happen");

	m_is_fullscreen = !ivideomode->IsWindowedMode();

	recompute();

	CConsole::the().info("VideoModeUtil initialized.");
}

void CVideoModeUtil::update()
{
	static uint32_t timer = GetTickCount();
	if (GetTickCount() - timer > 1000)
	{
		// see for window resize
		//static Vector2D last_screen_size = get_window_rect();
		//if (last_screen_size != get_window_rect())
		{
			recompute();
		//	last_screen_size = get_window_rect();
		}

		timer = GetTickCount();
	}
}

void CVideoModeUtil::recompute()
{
	// following information can be changed at runtime.

	auto ivideomode = *CMemoryHookMgr::the().IVideoMode().get();

	auto mode = ivideomode->GetCurrentMode();
	assert(mode && "This shouldn't happen");

	// get window size
	m_full_screen_size = get_window_rect();

	m_videomode_screen_size = { (float)mode->width, (float)mode->height };

	// windowed  : size of the window
	// fullscreen: blackbars on : size of the fullscreen window excluding blackbars
	//			   blackbars off: size of the fullscreen window.
	m_real_screen_size = m_full_screen_size;

	//
	// deal with blackbars
	//

	// You can ENABLE blackbars by NOT having these two in you launch options and when in fullscreen:
	bool stretch_aspect = CGenericUtil::the().does_cmdline_parameter_exist("-stretchaspect");
	bool no_fbo = CGenericUtil::the().does_cmdline_parameter_exist("-nofbo");
	m_has_blackbars = (!stretch_aspect && !no_fbo) && m_is_fullscreen;

	if (m_has_blackbars)
	{
		float height_aspect = m_full_screen_size.y / m_videomode_screen_size.y;
		float stretched_width = m_videomode_screen_size.x * height_aspect;
		float bar_width = (m_full_screen_size.x - stretched_width) / 2;

		// 1280x960
		// 1920x1080
		
		// 1080 / 960        = 1.125
		// 1280*1.25         = 1440
		// (1920 - 1440) / 2 = 240

		m_real_screen_size.x -= bar_width * 2.0f;
		
		m_ingame_viewport_pos = Vector2D(bar_width, 0);
	}
	else
	{
		m_ingame_viewport_pos = Vector2D(0, 0);
	}

#if 0
	CConsole::the().dinfo(__FUNCTION__);
	CConsole::the().dinfo("m_full_screen_size   : {}", m_full_screen_size);
	CConsole::the().dinfo("m_real_screen_size   : {}", m_real_screen_size);
	CConsole::the().dinfo("m_ingame_viewport_pos: {}", m_ingame_viewport_pos);
#endif
}

Vector2D CVideoModeUtil::get_window_rect()
{
	HWND hwnd = COxWareUI::the().get_window_handle();

	if (hwnd == NULL)
	{
		hwnd = ::GetDesktopWindow();
	}

	RECT r;
	::GetClientRect(hwnd, &r);
	return { (float)(r.right - r.left), (float)(r.bottom - r.top) };
}
