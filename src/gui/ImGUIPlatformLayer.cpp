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

IImGUIPlatformLayer* g_imgui_platform_layer_i = nullptr;

class CImGUIPlatformLayer : public IImGUIPlatformLayer
{
public:
	CImGUIPlatformLayer();
	~CImGUIPlatformLayer();

	bool create_new_layer(EImGUIPlatform plat, void* hwnd, const std::function<void()>& render_callback, ImGuiConfigFlags flags);
	void destroy_layer();

	void render();

	ImGUIBaseLayer* get_layer() { return m_platform_layer; }

	LRESULT imgui_wndproc_handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void override_cursor(EGUICursor to);
	void should_render_imgui_cursor(bool should_or_not) { m_should_render_cursor = should_or_not; }

	Vector2D get_screen_size();

private:
	// there can be only one per module.
	ImGUIBaseLayer* m_platform_layer = nullptr;

	EGUICursor m_current_cursor = GUICURSOR_Arrow;
	static ImGuiMouseCursor to_imgui_cursor(EGUICursor our)
	{
		switch (our)
		{
			case GUICURSOR_Arrow: return ImGuiMouseCursor_Arrow;
			case GUICURSOR_Hand: return ImGuiMouseCursor_Hand;
			case GUICURSOR_Input: return ImGuiMouseCursor_TextInput;
			case GUICURSOR_None: return ImGuiMouseCursor_None;
		}

		assert(0);
		return ImGuiMouseCursor_Arrow;
	}

	bool m_should_render_cursor = false;
};

CImGUIPlatformLayer g_imgui_platform_layer_mgr;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CImGUIPlatformLayer, IImGUIPlatformLayer, IIMGUIPLATFORMLAYER_INTERFACEID, g_imgui_platform_layer_mgr);

//-------------------------------------------------------------------------------
//
// CImGUIPlatformLayer implementation
// 
//-------------------------------------------------------------------------------

CImGUIPlatformLayer::CImGUIPlatformLayer()
{
	g_imgui_platform_layer_i = this;
}

CImGUIPlatformLayer::~CImGUIPlatformLayer()
{
	g_imgui_platform_layer_i = nullptr;
}

bool CImGUIPlatformLayer::create_new_layer(EImGUIPlatform plat, void* hwnd, const std::function<void()>& render_callback, ImGuiConfigFlags flags)
{
	static bool once = true;
	assert(once);
	once = false;

	switch (plat)
	{
		case IMPLATFORM_glfw:
		{
			m_platform_layer = new CImGUILayerGLFW(plat, hwnd, render_callback, flags);
			break;
		}
		case IMPLATFORM_win32:
		{
			m_platform_layer = new CImGUILayerWin32(plat, hwnd, render_callback, flags);
			break;
		}
		default:
		{
			assert(0 && "Invalid ImGui platform layer specified.");
			m_platform_layer = nullptr;
			return false;
		}
	}

	CConsole::the().info("Created new ImGUI platform layer: {}", m_platform_layer->get_name());
	m_platform_layer->initialize();

	return true;
}

void CImGUIPlatformLayer::destroy_layer()
{
	CConsole::the().info("Destroyed ImGUI platform layer: {}", m_platform_layer->get_name());
	m_platform_layer->destroy();
	m_platform_layer = nullptr;
}

void CImGUIPlatformLayer::render()
{
	m_current_cursor = GUICURSOR_Arrow;

	m_platform_layer->on_render(
		// post-render callback
		[&]()
		{
			//
			// mouse cursors
			//

			ImGui::GetIO().MouseDrawCursor = m_should_render_cursor;
			
			if (ImGui::GetMouseCursor() == ImGuiMouseCursor_Arrow)
			{
				ImGui::SetMouseCursor(to_imgui_cursor(m_current_cursor));
			}
		});
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CImGUIPlatformLayer::imgui_wndproc_handler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

void CImGUIPlatformLayer::override_cursor(EGUICursor to)
{
	m_current_cursor = to;
}

Vector2D CImGUIPlatformLayer::get_screen_size()
{
	return ImGui::GetIO().DisplaySize;
}
