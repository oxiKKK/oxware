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

#ifndef IIMGUIPLATFORMLAYER_H
#define IIMGUIPLATFORMLAYER_H
#pragma once

#include <functional>

enum EImGUIPlatform
{
	IMPLATFORM_none,

	IMPLATFORM_glfw,
	IMPLATFORM_win32,
};

// Each platform interface implements this (glfw or win32, ...) for a specific window
class ImGUIBaseLayer
{
public:
	ImGUIBaseLayer(EImGUIPlatform plat, void* hwnd, const std::function<void()>& render_callback, ImGuiConfigFlags flags) :
		m_plat(plat), m_window_handle(hwnd), m_render_callback(render_callback), m_flags(flags)
	{
	}

	virtual const char* get_name() const = 0;

	virtual bool initialize() = 0;
	virtual void destroy() = 0;

	virtual void on_render(const std::function<void()>& post_render_callback = NULL) = 0;

protected:
	void* m_window_handle;
	EImGUIPlatform m_plat;
	std::function<void()> m_render_callback;
	ImGuiConfigFlags m_flags;
};

enum EGUICursor
{
	GUICURSOR_Arrow, 
	GUICURSOR_Hand, 
	GUICURSOR_Input, 
	GUICURSOR_None, 
};

// interface that can be accessed from external modules
class IImGUIPlatformLayer : public IBaseInterface
{
public:
	virtual bool create_new_layer(EImGUIPlatform plat, void* hwnd, const std::function<void()>& render_callback, ImGuiConfigFlags flags = ImGuiConfigFlags_None) = 0;
	virtual void destroy_layer() = 0;

	virtual void render() = 0;

	virtual ImGUIBaseLayer* get_layer() = 0;

	virtual LRESULT imgui_wndproc_handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	virtual void override_cursor(EGUICursor to) = 0;
	virtual void should_render_imgui_cursor(bool should_or_not) = 0;

	// screen size
	virtual Vector2D get_screen_size() = 0;
};

extern IImGUIPlatformLayer* g_imgui_platform_layer_i;

#define IIMGUIPLATFORMLAYER_INTERFACEID "IImGUIPlatformLayer"

#endif // IIMGUIPLATFORMLAYER_H