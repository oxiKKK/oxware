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

// We'll use internal imgui here
#include <imgui_internal.h>

using namespace ImGui;

IGUIWindowRendering* g_gui_window_rendering_i = nullptr;

class CGUIWindowRendering : public IGUIWindowRendering
{
public:
	CGUIWindowRendering();
	~CGUIWindowRendering();

	ImDrawList* get_current_drawlist() { return ImGui::GetWindowDrawList(); }
	ImDrawList* get_background_drawlist() { return ImGui::GetBackgroundDrawList(); }
	ImDrawList* get_foreground_drawlist() { return ImGui::GetForegroundDrawList(); }

	void render_text(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what);
	void render_line(ImDrawList* current, const Vector2D& from, const Vector2D& to, const CColor& color, float thicc = 1.0f);
	
	void render_text_with_background(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what);

	// box without and with an outline
	void render_box(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding = 1.0f);
	void render_box_outline(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding = 1.0f,
							const CColor& outline_color = {}, float outline_thickness = 1.0f);

	void render_quad(ImDrawList* current, const Vector2D& top_left, const Vector2D& top_right, const Vector2D& bottom_right,
					 const Vector2D& bottom_left, const CColor& color);

	void render_circle(ImDrawList* current, const Vector2D& center, float radius, int num_segments, const CColor& color, float thicc = 1.0f);
};

CGUIWindowRendering g_gui_window_rendering;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGUIWindowRendering, IGUIWindowRendering, IGUIWINDOWRENDERING_INTERFACEID, g_gui_window_rendering);

//-------------------------------------------------------------------------------
//
// CGUIWindowRendering implementation
// 
//-------------------------------------------------------------------------------

CGUIWindowRendering::CGUIWindowRendering()
{
	g_gui_window_rendering_i = this;
}

CGUIWindowRendering::~CGUIWindowRendering()
{
	g_gui_window_rendering_i = nullptr;
}

void CGUIWindowRendering::render_text(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what)
{
	current->AddText(font, font->FontSize, at, color.as_u32(), what.c_str());
}

void CGUIWindowRendering::render_line(ImDrawList* current, const Vector2D& from, const Vector2D& to, const CColor& color, float thicc)
{
	current->AddLine(from, to, color.as_u32(), thicc);
}

void CGUIWindowRendering::render_text_with_background(ImDrawList* current, ImFont* font, const Vector2D& at, const CColor& color, const std::string& what)
{
	auto label_size = g_gui_fontmgr_i->calc_font_text_size(font, what.c_str());

	// background
	render_box(
		get_current_drawlist(),
		at,
		at + Vector2D(label_size.x, label_size.y),
		CColor(0, 0, 0, 70));

	// label
	render_text(
		get_current_drawlist(),
		font,
		at,
		CColor(255, 255, 255, 255),
		what);
}

void CGUIWindowRendering::render_box(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding)
{
	current->AddRectFilled(top_left, bottom_right, color.as_u32(), rounding);
}

void CGUIWindowRendering::render_box_outline(ImDrawList* current, const Vector2D& top_left, const Vector2D& bottom_right, const CColor& color, float rounding,
											 const CColor& outline_color, float outline_thickness)
{
	// contents
	current->AddRectFilled(top_left, bottom_right, color.as_u32(), rounding);

	// outline
	current->AddRect(top_left, bottom_right, outline_color.as_u32(), rounding, 0, outline_thickness);
}

void CGUIWindowRendering::render_quad(ImDrawList* current, const Vector2D& top_left, const Vector2D& top_right, 
									  const Vector2D& bottom_right, const Vector2D& bottom_left, const CColor& color)
{
	current->AddQuadFilled(top_left, top_right, bottom_right, bottom_left, color.as_u32());
}

void CGUIWindowRendering::render_circle(ImDrawList* current, const Vector2D& center, float radius, int num_segments, const CColor& color, float thicc)
{
	current->AddCircle(center, radius, color.as_u32(), num_segments, thicc);
}
