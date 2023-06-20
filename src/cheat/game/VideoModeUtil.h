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

#ifndef VIDEOMODEUTIL_H
#define VIDEOMODEUTIL_H
#pragma once

class CVideoModeUtil
{
public:
	DECL_BASIC_CLASS(CVideoModeUtil);

public:
	void initialize();
	void update();

	// getters
	inline bool is_fullscreen() const { return m_is_fullscreen; }
	inline bool has_blackbars() const { return m_has_blackbars; }

	// a little bit of information about screensize in this game:
	//	- when windowed  : m_full_screen_size == m_real_screen_size == size of the window
	//	- when fullscreen: m_full_screen_size is always the size of your monitor, and
	//					   when blackbars off: m_real_screen_size is the size of your monitor.
	//					   when blackbars on:  m_real_screen_size is the size of your monitor excluding the blackbars.
	inline Vector2D get_full_screen_size() const { return m_full_screen_size; }
	inline Vector2D get_real_screen_size() const { return m_real_screen_size; }

	// size of current videmode
	inline Vector2D get_videomode_screen_size() const { return m_videomode_screen_size; }

	inline Vector2D get_ingame_viewport_pos() const { return m_ingame_viewport_pos; }

private:
	void recompute();

	Vector2D get_window_rect();

private:
	bool m_is_fullscreen = false;
	bool m_has_blackbars = false;
	Vector2D m_full_screen_size, m_real_screen_size, m_videomode_screen_size;
	Vector2D m_ingame_viewport_pos;
};

#endif // VIDEOMODEUTIL_H