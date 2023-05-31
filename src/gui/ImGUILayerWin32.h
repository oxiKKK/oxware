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

#ifndef IMGUILAYERWIN32_H
#define IMGUILAYERWIN32_H
#pragma once

class CImGUILayerWin32 : public ImGUIBaseLayer
{
public:
	CImGUILayerWin32(EImGUIPlatform plat, void* hwnd, const std::function<void()>& render_callback, ImGuiConfigFlags flags)
		: ImGUIBaseLayer(plat, hwnd, render_callback , flags)
	{
	}

	const char* get_name() const { return "win32"; }

	bool initialize();
	void destroy();

	void on_render(const std::function<void()>& post_render_callback);

	inline HWND as_hwnd() const { return reinterpret_cast<HWND>(m_window_handle); }
};

#endif // IMGUILAYERWIN32_H